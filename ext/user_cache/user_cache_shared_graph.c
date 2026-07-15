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
   | Author: Go Kudo <zeriyoshi@php.net>.                                 |
   +----------------------------------------------------------------------+
*/

#include "user_cache_internal.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_enum.h"
#include "Zend/zend_operators.h"

#if (defined(__GNUC__) && ZEND_GCC_VERSION >= 4003) || __has_attribute(hot)
# define PHP_USER_CACHE_DECODE_HOT __attribute__((hot))
#else
# define PHP_USER_CACHE_DECODE_HOT
#endif

/* Request-local maps preserve identity for repeated graph nodes. */
#define PHP_USER_CACHE_DEFINE_DECODE_MAP(name, ctype, dtor, release_on_fail) \
	static zend_always_inline void user_cache_decode_##name##_map_teardown(void) \
	{ \
		if (UC_G(decode_##name##_map) != NULL) { \
			zend_hash_destroy(UC_G(decode_##name##_map)); \
			efree(UC_G(decode_##name##_map)); \
			UC_G(decode_##name##_map) = NULL; \
		} \
	} \
	\
	static zend_always_inline bool user_cache_decode_##name##_map_insert(uint32_t offset, ctype *entry) \
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
	static zend_always_inline ctype *user_cache_decode_##name##_map_find(uint32_t offset) \
	{ \
		if (UC_G(decode_##name##_map) == NULL) { \
			return NULL; \
		} \
		\
		return zend_hash_index_find_ptr(UC_G(decode_##name##_map), offset); \
	}

/* CALC and COPY must use the same route precedence. */
typedef enum {
	PHP_USER_CACHE_OBJECT_ROUTE_UNSTORABLE = 0,
	PHP_USER_CACHE_OBJECT_ROUTE_SAFE_DIRECT,
	PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE,
	PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS,
	PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE,
	PHP_USER_CACHE_OBJECT_ROUTE_SLEEP,
	PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP,
	PHP_USER_CACHE_OBJECT_ROUTE_SERDES,
	PHP_USER_CACHE_OBJECT_ROUTE_PLAIN
} php_user_cache_object_route;

typedef enum {
	PHP_USER_CACHE_COPY_OBJECT_REF_NEW = 0,
	PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED,
	PHP_USER_CACHE_COPY_OBJECT_REF_ERROR
} php_user_cache_copy_object_ref_result;

typedef bool (*php_user_cache_shared_graph_state_getter_t)(
	const zval *value,
	HashTable *state_memo,
	zval **state_ptr,
	zval *owned_state
);

static void user_cache_decode_array_dtor(zval *zv);
static void user_cache_decode_shape_prototype_dtor(zval *zv);
static void user_cache_shared_graph_refs_check_fork(void);
static bool user_cache_shared_graph_calc_value(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value
);
static bool user_cache_shared_graph_copy_value(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
);
static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_value(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
);
#if ZEND_DEBUG
static bool user_cache_shared_graph_rebase_verbatim_array(
	php_user_cache_shared_graph_rebase_context *ctx,
	zend_array *arr
);
#endif

static zend_always_inline bool user_cache_decode_range_ok(
	size_t buf_len,
	uint32_t offset,
	size_t need
)
{
	return offset <= buf_len && need <= buf_len - offset;
}

/* Check without overflowing size_t. */
static zend_always_inline bool user_cache_decode_array_range_ok(
	size_t buf_len,
	uint32_t offset,
	uint32_t count,
	size_t elem_size
)
{
	if (offset > buf_len) {
		return false;
	}

	return count <= (buf_len - offset) / elem_size;
}

static zend_always_inline bool user_cache_decode_fail_zval(zval *dst)
{
	zval_ptr_dtor(dst);

	ZVAL_UNDEF(dst);

	return false;
}

static zend_always_inline zend_string *user_cache_decode_string_at(
	const uint8_t *buf,
	size_t buf_len,
	uint32_t offset
)
{
	zend_string *string;

	if (!user_cache_decode_range_ok(buf_len, offset, _ZSTR_HEADER_SIZE)) {
		return NULL;
	}

	string = (zend_string *) (void *) (buf + offset);
	if (ZSTR_LEN(string) > buf_len ||
		!user_cache_decode_range_ok(
			buf_len,
			offset,
			_ZSTR_STRUCT_SIZE(ZSTR_LEN(string))
		)
	) {
		return NULL;
	}

	return string;
}

static zend_always_inline size_t user_cache_decode_node_header_size(uint8_t type)
{
	switch (type) {
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			return sizeof(php_user_cache_shared_graph_array);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			return sizeof(php_user_cache_shared_graph_shaped_array);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			return sizeof(php_user_cache_shared_graph_object);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			return sizeof(php_user_cache_shared_graph_safe_direct_object);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			return sizeof(php_user_cache_shared_graph_shaped_state_object);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return sizeof(php_user_cache_shared_graph_serdes_object);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM:
			return sizeof(php_user_cache_shared_graph_enum);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return sizeof(php_user_cache_shared_graph_reference);
		default:
			return 0;
	}
}

/* Address-keyed entries are valid while the decoded payload remains pinned. */
static zend_always_inline void *user_cache_decode_resolve_cache_find(const void *addr)
{
	uint32_t i;

	for (i = 0; i < PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
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

static zend_always_inline void user_cache_decode_resolve_cache_store(const void *addr, void *value)
{
	uint32_t slot;

	slot = UC_G(decode_resolve_direct_next)++ % PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS;
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

PHP_USER_CACHE_DEFINE_DECODE_MAP(
	identity,
	zend_object,
	php_user_cache_object_table_dtor,
	OBJ_RELEASE(entry)
)

PHP_USER_CACHE_DEFINE_DECODE_MAP(
	reference,
	zend_reference,
	php_user_cache_reference_table_dtor,
	if (GC_DELREF(entry) == 0) efree_size(entry, sizeof(zend_reference))
)

PHP_USER_CACHE_DEFINE_DECODE_MAP(
	array,
	zend_array,
	user_cache_decode_array_dtor,
	if (GC_DELREF(entry) == 0) zend_array_destroy(entry)
)

static zend_always_inline HashTable *user_cache_decode_shape_prototype_cache(void)
{
	if (UC_G(decode_shape_prototype_cache) == NULL) {
		UC_G(decode_shape_prototype_cache) = emalloc(sizeof(HashTable));
		zend_hash_init(
			UC_G(decode_shape_prototype_cache),
			8,
			NULL,
			user_cache_decode_shape_prototype_dtor,
			0
		);
	}

	return UC_G(decode_shape_prototype_cache);
}

static zend_always_inline size_t user_cache_shared_graph_alignment_padding(const void *buf)
{
	uintptr_t raw_addr, aligned_addr;

	raw_addr = (uintptr_t) buf;
	aligned_addr = (uintptr_t) ZEND_MM_ALIGNED_SIZE(raw_addr);

	return (size_t) (aligned_addr - raw_addr);
}

static zend_always_inline bool user_cache_shared_graph_header_is_valid(
	const php_user_cache_shared_graph_header *header
)
{
	return header->magic == PHP_USER_CACHE_SHARED_GRAPH_MAGIC &&
		header->version == PHP_USER_CACHE_SHARED_GRAPH_VERSION
	;
}

static zend_always_inline const uint8_t *user_cache_shared_graph_locate(
	const uint8_t *buf,
	size_t buf_len,
	size_t *graph_len)
{
	const php_user_cache_shared_graph_header *header;
	size_t padding;

	padding = user_cache_shared_graph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < sizeof(php_user_cache_shared_graph_header)) {
		return NULL;
	}

	buf += padding;
	buf_len -= padding;
	header = (const php_user_cache_shared_graph_header *) buf;
	if (!user_cache_shared_graph_header_is_valid(header)) {
		return NULL;
	}

	if (graph_len != NULL) {
		*graph_len = buf_len;
	}

	return buf;
}

static zend_always_inline void user_cache_shared_graph_calc_init(php_user_cache_shared_graph_calc_context *ctx)
{
	ctx->size = 0;
	ctx->reserve_failed = false;
	ctx->shared_verdicts = NULL;

	zend_hash_init(&ctx->seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_objects, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->seen_references, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->string_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->array_shape_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->state_schema_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_array_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);
}

static zend_always_inline void user_cache_shared_graph_calc_destroy(php_user_cache_shared_graph_calc_context *ctx)
{
	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->direct_array_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->array_shape_dedup);
	zend_hash_destroy(&ctx->string_dedup);
	zend_hash_destroy(&ctx->seen_references);
	zend_hash_destroy(&ctx->seen_objects);
	zend_hash_destroy(&ctx->seen_arrays);
}

static zend_always_inline bool user_cache_shared_graph_calc_reserve(
	php_user_cache_shared_graph_calc_context *ctx, size_t amount)
{
	size_t aligned_amount;

	aligned_amount = PHP_USER_CACHE_ALIGNED_SIZE(amount);
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

static zend_always_inline bool user_cache_shared_graph_calc_reserve_string(
	php_user_cache_shared_graph_calc_context *ctx,
	const zend_string *string)
{
	if (zend_hash_exists(&ctx->string_dedup, (zend_string *) string)) {
		return true;
	}

	if (!user_cache_shared_graph_calc_reserve(ctx, _ZSTR_STRUCT_SIZE(ZSTR_LEN(string)))) {
		return false;
	}

	return zend_hash_add_empty_element(&ctx->string_dedup, (zend_string *) string) != NULL;
}

static zend_always_inline void user_cache_shared_graph_key_append_u32(smart_str *key, uint32_t value)
{
	smart_str_appendl(key, (const char *) &value, sizeof(value));
}

static zend_always_inline bool user_cache_shared_graph_array_has_shape(const HashTable *arr)
{
	zend_string *key;

	if (arr->nNumOfElements == 0 ||
		HT_IS_PACKED(arr) ||
		arr->nNumOfElements > PHP_USER_CACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS ||
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

static zend_always_inline bool user_cache_shared_graph_can_restore_direct(zend_class_entry *ce)
{
	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		return false;
	}

	if (ce->type != ZEND_USER_CLASS && ce->create_object != NULL) {
		return false;
	}

	return true;
}

static zend_always_inline bool user_cache_shared_graph_can_use_verbatim_arrays(void)
{
#ifdef ZEND_WIN32
	/* Verbatim arrays contain absolute pointers and cannot cross mapping bases. */
	return false;
#else
	return !php_user_cache_active_context()->boundary_shared;
#endif
}

static zend_always_inline bool user_cache_shared_graph_is_unmangled_property_name(zend_string *prop_name)
{
	return ZSTR_LEN(prop_name) != 0 && ZSTR_VAL(prop_name)[0] != '\0';
}

/*
 * The state table pointer is captured before recursing into the state graph:
 * the memoized state zval lives in a HashTable whose bucket array is
 * reallocated when nested objects add their own memo entries, so a pointer
 * into that bucket must not be dereferenced afterwards. The referenced state
 * array itself is stable, so its HashTable is safe to hold across recursion.
 */
static zend_always_inline bool user_cache_shared_graph_safe_direct_property_shadows_state(
		zend_string *prop_name, const HashTable *state_ht)
{
	return state_ht != NULL &&
		user_cache_shared_graph_is_unmangled_property_name(prop_name) &&
		zend_hash_exists(state_ht, prop_name)
	;
}

static inline bool user_cache_class_overrides_safe_direct_magic_serialize(zend_class_entry *ce)
{
	zend_class_entry *base_ce = NULL;
	const php_user_cache_safe_direct_handlers *handlers;

	if (ce->type != ZEND_USER_CLASS ||
		ce->__serialize == NULL ||
		ce->__unserialize == NULL ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0
	) {
		return false;
	}

	handlers = php_user_cache_safe_direct_find_handlers(ce, &base_ce);

	return php_user_cache_class_overrides_safe_direct_magic_serialize_ex(ce, handlers, base_ce);
}

static zend_always_inline bool user_cache_shared_graph_can_use_safe_direct(zend_class_entry *ce)
{
	zend_class_entry *base_ce = php_user_cache_safe_direct_find_base(ce);

	return base_ce != NULL &&
		!user_cache_class_overrides_safe_direct_magic_serialize(ce) &&
		php_user_cache_safe_direct_state_serialize_func(ce) != NULL &&
		php_user_cache_safe_direct_state_unserialize_func(ce) != NULL
	;
}

static zend_always_inline bool user_cache_shared_graph_can_use_sleep_object(zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		(ce->serialize != NULL && ce->unserialize != NULL)
	) {
		return false;
	}

	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL;
}

static zend_always_inline bool user_cache_shared_graph_can_use_wakeup_object(zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS ||
		(ce->ce_flags & (ZEND_ACC_NOT_SERIALIZABLE|ZEND_ACC_ENUM)) != 0 ||
		ce->__serialize != NULL ||
		ce->__unserialize != NULL ||
		(ce->serialize != NULL && ce->unserialize != NULL) ||
		zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL
	) {
		return false;
	}

	return zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL;
}

static zend_always_inline bool user_cache_shared_graph_pointer_in_range(
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

#if ZEND_DEBUG
static zend_always_inline void *user_cache_shared_graph_rebase_pointer(
		void *ptr,
		const uint8_t *old_base,
		size_t len,
		ptrdiff_t delta)
{
	if (!user_cache_shared_graph_pointer_in_range(ptr, old_base, len)) {
		return ptr;
	}

	return (void *) ((char *) ptr - delta);
}
#endif

static zend_string *user_cache_shared_graph_array_shape_key(const HashTable *arr)
{
	zend_string *key;
	smart_str shape_key = {0};
	uint32_t count, key_len;

	ZEND_ASSERT(user_cache_shared_graph_array_has_shape(arr));

	count = (uint32_t) arr->nNumOfElements;
	smart_str_appendl(&shape_key, "ucshape", sizeof("ucshape") - 1);
	user_cache_shared_graph_key_append_u32(&shape_key, count);

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (ZSTR_LEN(key) > UINT32_MAX) {
			smart_str_free(&shape_key);

			return NULL;
		}

		key_len = (uint32_t) ZSTR_LEN(key);

		user_cache_shared_graph_key_append_u32(&shape_key, key_len);

		smart_str_appendl(&shape_key, ZSTR_VAL(key), ZSTR_LEN(key));
	} ZEND_HASH_FOREACH_END();

	return smart_str_extract(&shape_key);
}

static bool user_cache_shared_graph_calc_array_shape(
	php_user_cache_shared_graph_calc_context *ctx,
	const HashTable *arr,
	zend_string *shape_key
)
{
	zend_string *key;
	bool result = true, owns_shape_key = false;

	if (shape_key == NULL) {
		shape_key = user_cache_shared_graph_array_shape_key(arr);
		if (shape_key == NULL) {
			return false;
		}

		owns_shape_key = true;
	}

	if (zend_hash_exists(&ctx->array_shape_dedup, shape_key)) {
		if (owns_shape_key) {
			zend_string_release(shape_key);
		}

		return true;
	}

	if (!user_cache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_user_cache_shared_graph_array_shape)
		) ||
		!user_cache_shared_graph_calc_reserve(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(php_user_cache_shared_graph_array_shape_element)
		)
	) {
		if (owns_shape_key) {
			zend_string_release(shape_key);
		}

		return false;
	}

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		if (!user_cache_shared_graph_calc_reserve_string(ctx, key)) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (result) {
		result = zend_hash_add_empty_element(&ctx->array_shape_dedup, shape_key) != NULL;
	}

	if (owns_shape_key) {
		zend_string_release(shape_key);
	}

	return result;
}

static zend_string *user_cache_shared_graph_state_schema_key(
	zend_class_entry *ce,
	const HashTable *arr,
	zend_string **shape_key_out
)
{
	zend_string *shape_key;
	smart_str schema_key = {0};
	uint32_t class_name_len, shape_key_len;

	shape_key = user_cache_shared_graph_array_shape_key(arr);
	if (shape_key == NULL || ZSTR_LEN(ce->name) > UINT32_MAX || ZSTR_LEN(shape_key) > UINT32_MAX) {
		if (shape_key != NULL) {
			zend_string_release(shape_key);
		}

		return NULL;
	}

	class_name_len = (uint32_t) ZSTR_LEN(ce->name);
	shape_key_len = (uint32_t) ZSTR_LEN(shape_key);

	smart_str_appendl(&schema_key, "ucstate", sizeof("ucstate") - 1);
	user_cache_shared_graph_key_append_u32(&schema_key, class_name_len);
	smart_str_appendl(&schema_key, ZSTR_VAL(ce->name), ZSTR_LEN(ce->name));
	user_cache_shared_graph_key_append_u32(&schema_key, shape_key_len);
	smart_str_appendl(&schema_key, ZSTR_VAL(shape_key), ZSTR_LEN(shape_key));

	*shape_key_out = shape_key;

	return smart_str_extract(&schema_key);
}

static bool user_cache_shared_graph_calc_state_schema(
	php_user_cache_shared_graph_calc_context *ctx,
	zend_class_entry *ce,
	const HashTable *arr
)
{
	zend_string *schema_key, *shape_key = NULL;
	bool result;

	schema_key = user_cache_shared_graph_state_schema_key(ce, arr, &shape_key);
	if (schema_key == NULL) {
		return false;
	}

	if (zend_hash_exists(&ctx->state_schema_dedup, schema_key)) {
		zend_string_release(shape_key);
		zend_string_release(schema_key);

		return true;
	}

	result = user_cache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_user_cache_shared_graph_state_schema)
		) &&
		user_cache_shared_graph_calc_reserve_string(ctx, ce->name) &&
		user_cache_shared_graph_calc_array_shape(ctx, arr, shape_key)
	;

	zend_string_release(shape_key);

	if (result) {
		result = zend_hash_add_empty_element(&ctx->state_schema_dedup, schema_key) != NULL;
	}

	zend_string_release(schema_key);

	return result;
}

static bool user_cache_shared_graph_state_value_fits_schema(
	zval *value,
	const HashTable *state_array,
	HashTable *seen_arrs
)
{
	zend_ulong arr_key;
	zval *elem;

	if (php_user_cache_stack_overflowed()) {
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
			if (!php_user_cache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(value))) {
				return true;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), elem) {
				if (!user_cache_shared_graph_state_value_fits_schema(
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

static bool user_cache_shared_graph_state_array_fits_schema(const HashTable *arr)
{
	zval *elem;
	HashTable seen_arrs;
	bool result = true;

	if (!user_cache_shared_graph_array_has_shape(arr)) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
		if (!user_cache_shared_graph_state_value_fits_schema(
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

static bool user_cache_shared_graph_calc_shaped_state_object(
	php_user_cache_shared_graph_calc_context *ctx,
	zend_class_entry *ce,
	const HashTable *state_array
)
{
	zval *elem;

	if (!user_cache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_user_cache_shared_graph_shaped_state_object)
		) ||
		!user_cache_shared_graph_calc_reserve(
			ctx,
			(size_t) state_array->nNumOfElements * sizeof(php_user_cache_shared_graph_value)
		) ||
		!user_cache_shared_graph_calc_state_schema(ctx, ce, state_array)
	) {
		return false;
	}

	ZEND_HASH_FOREACH_VAL((HashTable *) state_array, elem) {
		if (!user_cache_shared_graph_calc_value(ctx, elem)) {
			return false;
		}
	} ZEND_HASH_FOREACH_END();

	return true;
}

static php_user_cache_object_route user_cache_shared_graph_classify_object_route_uncached(zend_class_entry *ce)
{
	if (user_cache_shared_graph_can_use_safe_direct(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_SAFE_DIRECT;
	}

	if (php_user_cache_class_uses_magic_serialize(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE;
	}

	if (php_user_cache_class_uses_serialize_props(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS;
	}

	if (php_user_cache_class_uses_magic_unserialize(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE;
	}

	if (user_cache_shared_graph_can_use_sleep_object(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_SLEEP;
	}

	if (user_cache_shared_graph_can_use_wakeup_object(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP;
	}

	if (php_user_cache_class_uses_serdes(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_SERDES;
	}

	if (!user_cache_shared_graph_can_restore_direct(ce)) {
		return PHP_USER_CACHE_OBJECT_ROUTE_UNSTORABLE;
	}

	return PHP_USER_CACHE_OBJECT_ROUTE_PLAIN;
}

/* The route is immutable per class and cached for the request. */
static php_user_cache_object_route user_cache_shared_graph_classify_object_route(zend_class_entry *ce)
{
	zval *cached, route_zv;
	php_user_cache_object_route route;

	if (UC_G(object_route_memo) != NULL) {
		cached = zend_hash_index_find(UC_G(object_route_memo), (zend_ulong) (uintptr_t) ce);
		if (cached != NULL) {
			return (php_user_cache_object_route) Z_LVAL_P(cached);
		}
	} else {
		UC_G(object_route_memo) = emalloc(sizeof(HashTable));
		zend_hash_init(UC_G(object_route_memo), 8, NULL, NULL, 0);
	}

	route = user_cache_shared_graph_classify_object_route_uncached(ce);

	ZVAL_LONG(&route_zv, (zend_long) route);
	zend_hash_index_add(UC_G(object_route_memo), (zend_ulong) (uintptr_t) ce, &route_zv);

	return route;
}

static void user_cache_shared_graph_object_route_memo_release(void)
{
	if (UC_G(object_route_memo) != NULL) {
		zend_hash_destroy(UC_G(object_route_memo));
		efree(UC_G(object_route_memo));

		UC_G(object_route_memo) = NULL;
	}
}

static bool user_cache_shared_graph_can_copy_verbatim_value(HashTable *seen_arrs, HashTable *direct_verdicts, const zval *value)
{
	const zval *packed_value;
	const HashTable *arr;
	const Bucket *bucket;
	zend_ulong arr_key;
	zval *cached, verdict;
	uint32_t i;
	bool result = true;

	if (php_user_cache_stack_overflowed()) {
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

			if (!php_user_cache_seen_test_and_add(seen_arrs, arr)) {
				return false;
			}

			if (HT_IS_PACKED(arr)) {
				for (i = 0; i < arr->nNumUsed; i++) {
					packed_value = &arr->arPacked[i];
					if (!user_cache_shared_graph_can_copy_verbatim_value(
							seen_arrs,
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
						!user_cache_shared_graph_can_copy_verbatim_value(
							seen_arrs,
							direct_verdicts,
							&bucket[i].val
						)
					) {
						result = false;

						break;
					}
				}
			}

			zend_hash_index_del(seen_arrs, arr_key);

			if (GC_REFCOUNT(arr) > 1) {
				ZVAL_BOOL(&verdict, result);
				zend_hash_index_add(direct_verdicts, arr_key, &verdict);
			}

			return result;
		default:
			return false;
	}
}

static bool user_cache_shared_graph_calc_verbatim_value(
		php_user_cache_shared_graph_calc_context *ctx,
		const zval *value)
{
	const zval *packed_value;
	const HashTable *arr;
	const Bucket *bucket;
	zend_ulong arr_key;
	zval *cached, seen_marker;
	uint32_t i;
	size_t data_size;
	bool result = true;

	if (php_user_cache_stack_overflowed()) {
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
			return user_cache_shared_graph_calc_reserve_string(ctx, Z_STR_P(value));
		case IS_ARRAY:
			arr = Z_ARRVAL_P(value);
			if (arr->nNumOfElements == 0) {
				return true;
			}

			arr_key = (zend_ulong) (uintptr_t) arr;
			if (GC_REFCOUNT(arr) > 1) {
				cached = zend_hash_index_find(&ctx->direct_array_dedup, arr_key);
				if (cached != NULL) {
					return true;
				}
			}

			if (!php_user_cache_seen_test_and_add(&ctx->seen_arrays, arr)) {
				return false;
			}

			data_size = HT_IS_PACKED(arr) ? HT_PACKED_USED_SIZE(arr) : HT_USED_SIZE(arr);
			if (!user_cache_shared_graph_calc_reserve(ctx, sizeof(zend_array)) ||
				!user_cache_shared_graph_calc_reserve(ctx, data_size)
			) {
				result = false;

				goto done;
			}

			if (HT_IS_PACKED(arr)) {
				for (i = 0; i < arr->nNumUsed; i++) {
					packed_value = &arr->arPacked[i];
					if (!user_cache_shared_graph_calc_verbatim_value(ctx, packed_value)) {
						result = false;

						break;
					}
				}
			} else {
				bucket = arr->arData;
				for (i = 0; i < arr->nNumUsed; i++) {
					if (bucket[i].key != NULL &&
						!user_cache_shared_graph_calc_reserve_string(ctx, bucket[i].key)
					) {
						result = false;

						break;
					}

					if (Z_TYPE(bucket[i].val) != IS_UNDEF &&
						!user_cache_shared_graph_calc_verbatim_value(ctx, &bucket[i].val)
					) {
						result = false;

						break;
					}
				}
			}

done:
			zend_hash_index_del(&ctx->seen_arrays, arr_key);

			if (result && GC_REFCOUNT(arr) > 1) {
				ZVAL_TRUE(&seen_marker);
				zend_hash_index_add(&ctx->direct_array_dedup, arr_key, &seen_marker);
			}

			return result;
		default:
			return false;
	}
}

static bool user_cache_shared_graph_produce_safe_direct_state(const zval *value, zval *state)
{
	php_user_cache_safe_direct_state_serialize_func_t serialize_func;

	serialize_func = php_user_cache_safe_direct_state_serialize_func(Z_OBJCE_P(value));
	if (serialize_func == NULL ||
		!serialize_func(state, value) ||
		Z_TYPE_P(state) != IS_ARRAY
	) {
		if (!Z_ISUNDEF_P(state)) {
			zval_ptr_dtor(state);
			ZVAL_UNDEF(state);
		}

		return false;
	}

	return true;
}

static bool user_cache_shared_graph_extract_serialize_snapshot(const zval *value, zval *state)
{
	return php_user_cache_serdes_call_magic_serialize(Z_OBJ_P(value), state);
}

static bool user_cache_shared_graph_extract_property_snapshot(const zval *value, zval *state)
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

static bool user_cache_shared_graph_extract_sleep_snapshot(const zval *value, zval *state)
{
	const char *msg = NULL;

	if (!php_user_cache_serdes_get_sleep_state((zval *) value, state, &msg)) {
		if (msg != NULL) {
			zend_type_error("%s", msg);
		}

		return false;
	}

	return true;
}

static bool user_cache_shared_graph_extract_unserialize_route_snapshot(const zval *value, zval *state)
{
	if (zend_hash_find_known_hash(&Z_OBJCE_P(value)->function_table, ZSTR_KNOWN(ZEND_STR_SLEEP)) != NULL) {
		return user_cache_shared_graph_extract_sleep_snapshot(value, state);
	}

	return user_cache_shared_graph_extract_property_snapshot(value, state);
}

/* A NULL memo means the write lock is held, so state hooks may not run. */
static bool user_cache_shared_graph_get_memoized_state(
		const zval *value,
		HashTable *state_memo,
		bool (*produce_state)(const zval *value, zval *state),
		zval **state_ptr,
		zval *owned_state)
{
	zend_ulong memo_key;
	zval *memo_state;

	ZVAL_UNDEF(owned_state);
	*state_ptr = NULL;

	if (state_memo == NULL) {
		return false;
	}

	memo_key = (zend_ulong) (uintptr_t) Z_OBJ_P(value);
	memo_state = zend_hash_index_find(state_memo, memo_key);
	if (memo_state != NULL) {
		if (Z_TYPE_P(memo_state) != IS_ARRAY) {
			return false;
		}

		*state_ptr = memo_state;

		return true;
	}

	if (!produce_state(value, owned_state)) {
		if (!Z_ISUNDEF_P(owned_state)) {
			zval_ptr_dtor(owned_state);
			ZVAL_UNDEF(owned_state);
		}

		return false;
	}

	memo_state = zend_hash_index_add(state_memo, memo_key, owned_state);
	if (memo_state == NULL) {
		zval_ptr_dtor(owned_state);
		ZVAL_UNDEF(owned_state);

		return false;
	}

	ZVAL_UNDEF(owned_state);
	*state_ptr = memo_state;

	return true;
}

/* Safe-direct state may be produced under the write lock. */
static bool user_cache_shared_graph_get_safe_direct_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	if (state_memo != NULL) {
		return user_cache_shared_graph_get_memoized_state(
			value,
			state_memo,
			user_cache_shared_graph_produce_safe_direct_state,
			state_ptr,
			owned_state
		);
	}

	ZVAL_UNDEF(owned_state);
	*state_ptr = NULL;

	if (!user_cache_shared_graph_produce_safe_direct_state(value, owned_state)) {
		if (!Z_ISUNDEF_P(owned_state)) {
			zval_ptr_dtor(owned_state);
			ZVAL_UNDEF(owned_state);
		}

		return false;
	}

	*state_ptr = owned_state;

	return true;
}

static bool user_cache_shared_graph_get_magic_serialized_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	return user_cache_shared_graph_get_memoized_state(
		value,
		state_memo,
		user_cache_shared_graph_extract_serialize_snapshot,
		state_ptr,
		owned_state
	);
}

static bool user_cache_shared_graph_get_magic_unserialized_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	return user_cache_shared_graph_get_memoized_state(
		value,
		state_memo,
		user_cache_shared_graph_extract_unserialize_route_snapshot,
		state_ptr,
		owned_state
	);
}

static bool user_cache_shared_graph_get_sleep_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	return user_cache_shared_graph_get_memoized_state(
		value,
		state_memo,
		user_cache_shared_graph_extract_sleep_snapshot,
		state_ptr,
		owned_state
	);
}

static bool user_cache_shared_graph_get_wakeup_state(
		const zval *value,
		HashTable *state_memo,
		zval **state_ptr,
		zval *owned_state)
{
	return user_cache_shared_graph_get_memoized_state(
		value,
		state_memo,
		user_cache_shared_graph_extract_property_snapshot,
		state_ptr,
		owned_state
	);
}

static bool user_cache_shared_graph_get_serdes_blob(
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

	/* Serialization may invoke user code and is forbidden under the write lock. */
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

	if (!php_user_cache_serdes_encode((zval *) value, &buf, &msg)) {
		if (msg != NULL) {
			zend_type_error("%s", msg);
		}

		smart_str_free(&buf);

		return false;
	}

	blob = smart_str_extract(&buf);
	if (ZSTR_LEN(blob) == 0 || ZSTR_LEN(blob) > UINT32_MAX) {
		zend_string_release(blob);

		return false;
	}

	ZVAL_STR(&blob_zv, blob);
	memo_state = zend_hash_index_add(state_memo, memo_key, &blob_zv);
	if (memo_state == NULL) {
		zend_string_release(blob);

		return false;
	}

	*blob_ptr = Z_STR_P(memo_state);

	return true;
}

static php_user_cache_shared_graph_state_getter_t user_cache_shared_graph_route_state_getter(
	php_user_cache_object_route route)
{
	switch (route) {
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
		case PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return user_cache_shared_graph_get_magic_serialized_state;
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return user_cache_shared_graph_get_magic_unserialized_state;
		case PHP_USER_CACHE_OBJECT_ROUTE_SLEEP:
			return user_cache_shared_graph_get_sleep_state;
		case PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP:
			return user_cache_shared_graph_get_wakeup_state;
		default:
			return NULL;
	}
}

static bool user_cache_shared_graph_calc_magic_state_object(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value,
	zend_object *obj,
	php_user_cache_object_route route)
{
	php_user_cache_shared_graph_state_getter_t get_state;
	zval *state_ptr, state;
	bool result;

	/* Reserve shared objects only once. */
	if (!php_user_cache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	get_state = user_cache_shared_graph_route_state_getter(route);
	if (!get_state(value, ctx->state_memo, &state_ptr, &state)) {
		return false;
	}

	if (user_cache_shared_graph_state_array_fits_schema(Z_ARRVAL_P(state_ptr))) {
		result = user_cache_shared_graph_calc_shaped_state_object(
			ctx,
			obj->ce,
			Z_ARRVAL_P(state_ptr)
		);
	} else {
		result = user_cache_shared_graph_calc_reserve(ctx,
			sizeof(php_user_cache_shared_graph_safe_direct_object)) &&
			user_cache_shared_graph_calc_reserve_string(ctx, obj->ce->name) &&
			user_cache_shared_graph_calc_value(ctx, state_ptr)
		;
	}

	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool user_cache_shared_graph_calc_sleep_state_object(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value,
	zend_object *obj,
	php_user_cache_object_route route)
{
	php_user_cache_shared_graph_state_getter_t get_state;
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *state_ptr, state, *prop_val;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!php_user_cache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	get_state = user_cache_shared_graph_route_state_getter(route);
	if (!get_state(value, ctx->state_memo, &state_ptr, &state)) {
		return false;
	}

	props = Z_ARRVAL_P(state_ptr);
	if (user_cache_shared_graph_state_array_fits_schema(props)) {
		result = user_cache_shared_graph_calc_shaped_state_object(
			ctx,
			obj->ce,
			props
		);
	} else {
		prop_count = zend_hash_num_elements(props);
		result = user_cache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_user_cache_shared_graph_object)
			) &&
			user_cache_shared_graph_calc_reserve_string(ctx, obj->ce->name) &&
			(prop_count == 0 ||
				user_cache_shared_graph_calc_reserve(
					ctx,
					(size_t) prop_count * sizeof(php_user_cache_shared_graph_property)
				)
			)
		;

		if (result) {
			/* Native serialization restores integer keys as property names. */
			ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
				resolved_name = prop_name != NULL
					? zend_string_copy(prop_name)
					: zend_long_to_str((zend_long) num_key)
				;

				if (!user_cache_shared_graph_calc_reserve_string(ctx, resolved_name) ||
					!user_cache_shared_graph_calc_value(ctx, prop_val)
				) {
					zend_string_release(resolved_name);
					result = false;

					break;
				}

				zend_string_release(resolved_name);
			} ZEND_HASH_FOREACH_END();
		}
	}

	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool user_cache_shared_graph_calc_safe_direct_object(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value,
	zend_object *obj)
{
	zend_string *prop_name;
	zval *prop_val, *src_val, *state_ptr, state;
	const HashTable *state_ht;
	HashTable *props;
	uint32_t prop_count;
	bool result;

	if (!user_cache_shared_graph_get_safe_direct_state(
			value,
			ctx->state_memo,
			&state_ptr,
			&state
		)
	) {
		return false;
	}

	/* state_ptr may dangle once recursion resizes the memo; keep the array. */
	state_ht = Z_TYPE_P(state_ptr) == IS_ARRAY ? Z_ARRVAL_P(state_ptr) : NULL;

	/* Reserve shared objects only once. */
	if (!php_user_cache_seen_test_and_add(&ctx->seen_objects, obj)) {
		result = true;

		goto cleanup;
	}

	if (!user_cache_shared_graph_calc_reserve(ctx,
		sizeof(php_user_cache_shared_graph_safe_direct_object)) ||
		!user_cache_shared_graph_calc_reserve_string(ctx, obj->ce->name) ||
		!user_cache_shared_graph_calc_value(ctx, state_ptr)
	) {
		result = false;

		goto cleanup;
	}

	props = zend_std_get_properties(obj);
	prop_count = props != NULL ? props->nNumOfElements : 0;
	if (prop_count != 0 &&
		!user_cache_shared_graph_calc_reserve(
			ctx,
			(size_t) prop_count * sizeof(php_user_cache_shared_graph_property)
		)
	) {
		result = false;

		goto cleanup;
	}

	result = true;
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

			if (user_cache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					state_ht
				)
			) {
				continue;
			}

			if (!user_cache_shared_graph_calc_reserve_string(ctx, prop_name) ||
				!user_cache_shared_graph_calc_value(ctx, src_val)
			) {
				result = false;

				break;
			}
		} ZEND_HASH_FOREACH_END();
	}

cleanup:
	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool user_cache_shared_graph_calc_plain_object(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value,
	zend_object *obj)
{
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	uint32_t prop_count;

	if (!php_user_cache_seen_test_and_add(&ctx->seen_objects, obj)) {
		return true;
	}

	if (!user_cache_shared_graph_calc_reserve(
			ctx,
			sizeof(php_user_cache_shared_graph_object)
		) ||
		!user_cache_shared_graph_calc_reserve_string(ctx, obj->ce->name)
	) {
		return false;
	}

	props = zend_get_properties_for((zval *) value, ZEND_PROP_PURPOSE_SERIALIZE);
	prop_count = props != NULL ? props->nNumOfElements : 0;
	if (prop_count != 0 &&
		!user_cache_shared_graph_calc_reserve(
			ctx,
			(size_t) prop_count * sizeof(php_user_cache_shared_graph_property)
		)
	) {
		if (props != NULL) {
			zend_release_properties(props);
		}

		return false;
	}

	if (props != NULL) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			if (prop_name == NULL) {
				zend_release_properties(props);

				return false;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (!user_cache_shared_graph_calc_reserve_string(ctx, prop_name)) {
				zend_release_properties(props);

				return false;
			}

			if (!user_cache_shared_graph_calc_value(ctx, src_val)) {
				zend_release_properties(props);

				return false;
			}
		} ZEND_HASH_FOREACH_END();

		zend_release_properties(props);
	}

	return true;
}

static bool user_cache_shared_graph_calc_object(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value
)
{
	zend_string *case_name, *serdes_blob;
	zend_class_entry *ce;
	zend_object *obj;

	obj = Z_OBJ_P(value);
	ce = obj->ce;
	if (ce == zend_ce_closure) {
		zend_type_error(PHP_USER_CACHE_MSG_CLOSURE_UNSTORABLE);

		return false;
	}

	/* Do not initialize lazy objects while walking serialized state. */
	if (zend_object_is_lazy(obj)) {
		zend_type_error(PHP_USER_CACHE_MSG_LAZY_OBJECT_UNSTORABLE);

		return false;
	}

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		case_name = Z_STR_P(zend_enum_fetch_case_name(obj));

		return user_cache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_user_cache_shared_graph_enum)
			) &&
			user_cache_shared_graph_calc_reserve_string(ctx, ce->name) &&
			user_cache_shared_graph_calc_reserve_string(ctx, case_name)
		;
	}

	switch (user_cache_shared_graph_classify_object_route(ce)) {
		case PHP_USER_CACHE_OBJECT_ROUTE_SAFE_DIRECT:
			return user_cache_shared_graph_calc_safe_direct_object(ctx, value, obj);
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
			return user_cache_shared_graph_calc_magic_state_object(
				ctx, value, obj, PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return user_cache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return user_cache_shared_graph_calc_magic_state_object(
				ctx, value, obj, PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SLEEP:
			return user_cache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_USER_CACHE_OBJECT_ROUTE_SLEEP
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP:
			return user_cache_shared_graph_calc_sleep_state_object(
				ctx, value, obj, PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SERDES:
			if (!php_user_cache_seen_test_and_add(&ctx->seen_objects, obj)) {
				return true;
			}

			if (!user_cache_shared_graph_get_serdes_blob(value, ctx->state_memo, &serdes_blob)) {
				return false;
			}

			return user_cache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_user_cache_shared_graph_serdes_object) + ZSTR_LEN(serdes_blob)
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_PLAIN:
			return user_cache_shared_graph_calc_plain_object(ctx, value, obj);
		case PHP_USER_CACHE_OBJECT_ROUTE_UNSTORABLE:
			return false;
	}

	return false;
}

static bool user_cache_shared_graph_calc_array(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value
)
{
	const HashTable *arr;
	zend_ulong arr_key;
	zend_string *key;
	zval *elem, *verdict, verdict_zv;
	bool result, verbatim;

	arr = Z_ARRVAL_P(value);

	if (arr->nNumOfElements == 0) {
		if (arr->nNextFreeElement != 0) {
			return user_cache_shared_graph_calc_reserve(ctx,
				sizeof(php_user_cache_shared_graph_array))
			;
		}

		return true;
	}

	if (ctx->verbatim_arrays_allowed) {
		if (GC_FLAGS(arr) & IS_ARRAY_IMMUTABLE) {
			return user_cache_shared_graph_calc_verbatim_value(ctx, value);
		}

		/* A state hook may free an array keyed by address in the verdict map. */
		verdict = NULL;
		if (ctx->shared_verdicts != NULL &&
			(ctx->state_memo == NULL || zend_hash_num_elements(ctx->state_memo) == 0)
		) {
			verdict = zend_hash_index_find(ctx->shared_verdicts, (zend_ulong) (uintptr_t) arr);
		}

		if (verdict != NULL) {
			verbatim = Z_TYPE_P(verdict) == IS_TRUE;
		} else {
			verbatim = user_cache_shared_graph_can_copy_verbatim_value(
				&ctx->seen_arrays,
				&ctx->direct_verdicts,
				value
			);

			if (ctx->shared_verdicts != NULL) {
				ZVAL_BOOL(&verdict_zv, verbatim);
				zend_hash_index_add(ctx->shared_verdicts, (zend_ulong) (uintptr_t) arr, &verdict_zv);
			}
		}

		if (verbatim) {
			return user_cache_shared_graph_calc_verbatim_value(ctx, value);
		}
	}

	if (!php_user_cache_seen_test_and_add(&ctx->seen_arrays, arr)) {
		return true;
	}

	arr_key = (zend_ulong) (uintptr_t) arr;
	result = true;

	if (user_cache_shared_graph_array_has_shape(arr)) {
		if (!user_cache_shared_graph_calc_reserve(
				ctx,
				sizeof(php_user_cache_shared_graph_shaped_array)
			) ||
			!user_cache_shared_graph_calc_reserve(
				ctx,
				(size_t) arr->nNumOfElements * sizeof(php_user_cache_shared_graph_value)
			) ||
			!user_cache_shared_graph_calc_array_shape(ctx, arr, NULL)
		) {
			result = false;

			goto done;
		}

		ZEND_HASH_FOREACH_VAL((HashTable *) arr, elem) {
			if (!user_cache_shared_graph_calc_value(ctx, elem)) {
				result = false;

				break;
			}
		} ZEND_HASH_FOREACH_END();

		goto done;
	}

	if (!user_cache_shared_graph_calc_reserve(ctx, sizeof(php_user_cache_shared_graph_array)) ||
		!user_cache_shared_graph_calc_reserve(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(php_user_cache_shared_graph_array_element)
		)
	) {
		result = false;

		goto done;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL((HashTable *) arr, key, elem) {
		if (key != NULL && !user_cache_shared_graph_calc_reserve_string(ctx, key)) {
			result = false;

			break;
		}

		if (!user_cache_shared_graph_calc_value(ctx, elem)) {
			result = false;

			break;
		}
	} ZEND_HASH_FOREACH_END();

done:
	zend_hash_index_del(&ctx->seen_arrays, arr_key);

	return result;
}

static bool user_cache_shared_graph_calc_value(
	php_user_cache_shared_graph_calc_context *ctx,
	const zval *value
)
{
	zend_reference *ref;

	if (php_user_cache_stack_overflowed()) {
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
			return user_cache_shared_graph_calc_reserve_string(ctx, Z_STR_P(value));
		case IS_RESOURCE:
			zend_type_error(PHP_USER_CACHE_MSG_RESOURCE_UNSTORABLE);

			return false;
		case IS_ARRAY:
			return user_cache_shared_graph_calc_array(ctx, value);
		case IS_OBJECT:
			return user_cache_shared_graph_calc_object(ctx, value);
		case IS_REFERENCE:
			ref = Z_REF_P(value);

			if (!php_user_cache_seen_test_and_add(&ctx->seen_references, ref)) {
				return true;
			}

			if (!user_cache_shared_graph_calc_reserve(ctx,
				sizeof(php_user_cache_shared_graph_reference))
			) {
				return false;
			}

			return user_cache_shared_graph_calc_value(ctx, &ref->val);
		default:
			return false;
	}
}

static void user_cache_shared_graph_copy_init(
	php_user_cache_shared_graph_copy_context *ctx,
	uint8_t *buf,
	size_t size
)
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
	zend_hash_init(&ctx->direct_array_dedup, 8, NULL, NULL, 0);
	zend_hash_init(&ctx->direct_verdicts, 8, NULL, NULL, 0);

	ctx->has_shared_identity = false;
	ctx->has_object = false;
	ctx->prefers_prototype = false;
	ctx->has_userland_restore_object = false;
	ctx->has_verbatim_array = false;
	ctx->verbatim_arrays_allowed = user_cache_shared_graph_can_use_verbatim_arrays();
	ctx->shared_verdicts = NULL;
}

static void user_cache_shared_graph_copy_destroy(php_user_cache_shared_graph_copy_context *ctx)
{
	if (ctx->fixup_offsets != NULL) {
		efree(ctx->fixup_offsets);

		ctx->fixup_offsets = NULL;
	}

	zend_hash_destroy(&ctx->direct_verdicts);
	zend_hash_destroy(&ctx->direct_array_dedup);
	zend_hash_destroy(&ctx->state_schema_dedup);
	zend_hash_destroy(&ctx->array_shape_dedup);
	zend_hash_destroy(&ctx->string_dedup);
	zend_hash_destroy(&ctx->seen_references);
	zend_hash_destroy(&ctx->seen_objects);
	zend_hash_destroy(&ctx->seen_arrays);
}

/* Pack the flags displacement into the two free alignment bits. */
static bool user_cache_shared_graph_seen_record_object_offsets(
	php_user_cache_shared_graph_copy_context *ctx,
	const zend_object *obj,
	uint32_t obj_offset,
	uint32_t flags_offset
)
{
	uint32_t delta;

	delta = flags_offset - obj_offset;

	ZEND_ASSERT((obj_offset & 3) == 0);
	ZEND_ASSERT((delta == 4 || delta == 12) && "unexpected reserved/flags field offset");

	return zend_hash_index_add_ptr(
		&ctx->seen_objects,
		(zend_ulong) (uintptr_t) obj,
		(void *) (uintptr_t) (obj_offset | (delta >> 2))
	) != NULL;
}

static bool user_cache_shared_graph_seen_promote_object_to_shared(
	php_user_cache_shared_graph_copy_context *ctx,
	const zend_object *obj,
	uint32_t *obj_offset
)
{
	void *seen_offset;
	uint32_t packed, flags_offset;

	seen_offset = zend_hash_index_find_ptr(&ctx->seen_objects, (zend_ulong) (uintptr_t) obj);
	if (seen_offset == NULL) {
		return false;
	}

	packed = (uint32_t) (uintptr_t) seen_offset;
	*obj_offset = packed & ~(uint32_t) 3;
	flags_offset = *obj_offset + ((packed & 3) << 2);

	*(uint32_t *) (ctx->buffer + flags_offset) |= PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED;
	ctx->has_shared_identity = true;

	return true;
}

static php_user_cache_copy_object_ref_result user_cache_shared_graph_copy_emit_object_ref_if_seen(
	php_user_cache_shared_graph_copy_context *ctx,
	const zend_object *obj,
	php_user_cache_shared_graph_value *dst
)
{
	uint32_t shared_offset;

	if (zend_hash_index_find_ptr(&ctx->seen_objects, (zend_ulong) (uintptr_t) obj) == NULL) {
		return PHP_USER_CACHE_COPY_OBJECT_REF_NEW;
	}

	if (!user_cache_shared_graph_seen_promote_object_to_shared(ctx, obj, &shared_offset)) {
		return PHP_USER_CACHE_COPY_OBJECT_REF_ERROR;
	}

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF;
	dst->payload.offset = shared_offset;

	return PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
}

/* Callers must initialize the full unaligned region. */
static bool user_cache_shared_graph_copy_alloc(
	php_user_cache_shared_graph_copy_context *ctx,
	size_t amount,
	uint32_t *offset
)
{
	size_t aligned_amount;

	aligned_amount = PHP_USER_CACHE_ALIGNED_SIZE(amount);
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

/* Record absolute pointer slots for relocation. */
static zend_always_inline void user_cache_shared_graph_copy_record_fixup(
		php_user_cache_shared_graph_copy_context *ctx,
		const void *slot)
{
	ZEND_ASSERT(user_cache_shared_graph_pointer_in_range(slot, ctx->buffer, ctx->size));
	ZEND_ASSERT(((uintptr_t) slot & (sizeof(uintptr_t) - 1)) == 0);

	if (ctx->fixup_count == ctx->fixup_capacity) {
		ctx->fixup_capacity = ctx->fixup_capacity == 0 ? 8 : ctx->fixup_capacity * 2;
		ctx->fixup_offsets = erealloc(ctx->fixup_offsets, sizeof(*ctx->fixup_offsets) * ctx->fixup_capacity);
	}

	ctx->fixup_offsets[ctx->fixup_count++] = (uint32_t) ((const uint8_t *) slot - ctx->buffer);
}

static bool user_cache_shared_graph_copy_string(
	php_user_cache_shared_graph_copy_context *ctx,
	const zend_string *str,
	uint32_t *offset
)
{
	zend_string *new_str;
	zval *cached, cached_offset;
	uint32_t str_offset;
	size_t str_size;

	cached = zend_hash_find(&ctx->string_dedup, (zend_string *) str);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		return true;
	}

	str_size = _ZSTR_STRUCT_SIZE(ZSTR_LEN(str));
	if (!user_cache_shared_graph_copy_alloc(ctx, str_size, &str_offset)) {
		return false;
	}

	new_str = (zend_string *) (ctx->buffer + str_offset);

	memcpy(new_str, str, str_size);

	GC_SET_REFCOUNT(new_str, 2);
	GC_TYPE_INFO(new_str) = GC_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT);
	*offset = str_offset;

	ZVAL_LONG(&cached_offset, (zend_long) str_offset);
	zend_hash_add(&ctx->string_dedup, (zend_string *) str, &cached_offset);

	return true;
}

static bool user_cache_shared_graph_copy_array_shape(
	php_user_cache_shared_graph_copy_context *ctx,
	const HashTable *arr,
	uint32_t *offset,
	zend_string *shape_key
)
{
	php_user_cache_shared_graph_array_shape *graph_shape;
	php_user_cache_shared_graph_array_shape_element *shape_elems, *shape_elem;
	zend_string *key;
	zval *cached, cached_offset;
	uint32_t shape_offset, elems_offset, key_offset;
	bool owns_shape_key = false;

	if (shape_key == NULL) {
		shape_key = user_cache_shared_graph_array_shape_key(arr);
		if (shape_key == NULL) {
			return false;
		}

		owns_shape_key = true;
	}

	cached = zend_hash_find(&ctx->array_shape_dedup, shape_key);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);

		if (owns_shape_key) {
			zend_string_release(shape_key);
		}

		return true;
	}

	if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*graph_shape), &shape_offset) ||
		!user_cache_shared_graph_copy_alloc(
			ctx,
			(size_t) arr->nNumOfElements * sizeof(*shape_elems),
			&elems_offset
		)
	) {
		if (owns_shape_key) {
			zend_string_release(shape_key);
		}

		return false;
	}

	graph_shape = (php_user_cache_shared_graph_array_shape *) (ctx->buffer + shape_offset);
	graph_shape->count = (uint32_t) arr->nNumOfElements;
	graph_shape->elements_offset = elems_offset;
	graph_shape->reserved = 0;
	graph_shape->reserved2 = 0;

	shape_elems = (php_user_cache_shared_graph_array_shape_element *) (ctx->buffer + elems_offset);
	shape_elem = shape_elems;

	ZEND_HASH_FOREACH_STR_KEY((HashTable *) arr, key) {
		ZEND_ASSERT(key != NULL);

		if (!user_cache_shared_graph_copy_string(ctx, key, &key_offset)) {
			if (owns_shape_key) {
				zend_string_release(shape_key);
			}

			return false;
		}

		shape_elem->key_offset = key_offset;

		++shape_elem;
	} ZEND_HASH_FOREACH_END();

	ZVAL_LONG(&cached_offset, (zend_long) shape_offset);
	if (zend_hash_add(&ctx->array_shape_dedup, shape_key, &cached_offset) == NULL) {
		if (owns_shape_key) {
			zend_string_release(shape_key);
		}

		return false;
	}

	*offset = shape_offset;

	if (owns_shape_key) {
		zend_string_release(shape_key);
	}

	return true;
}

static bool user_cache_shared_graph_copy_state_schema(
	php_user_cache_shared_graph_copy_context *ctx,
	zend_class_entry *ce,
	const HashTable *arr,
	uint32_t *offset
)
{
	php_user_cache_shared_graph_state_schema *schema;
	zend_string *schema_key, *shape_key = NULL;
	zval *cached, cached_offset;
	uint32_t schema_offset, class_name_offset, shape_offset;

	schema_key = user_cache_shared_graph_state_schema_key(ce, arr, &shape_key);
	if (schema_key == NULL) {
		return false;
	}

	cached = zend_hash_find(&ctx->state_schema_dedup, schema_key);
	if (cached != NULL) {
		*offset = (uint32_t) Z_LVAL_P(cached);
		zend_string_release(shape_key);
		zend_string_release(schema_key);

		return true;
	}

	if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*schema), &schema_offset) ||
		!user_cache_shared_graph_copy_string(ctx, ce->name, &class_name_offset) ||
		!user_cache_shared_graph_copy_array_shape(ctx, arr, &shape_offset, shape_key)
	) {
		zend_string_release(shape_key);
		zend_string_release(schema_key);

		return false;
	}

	schema = (php_user_cache_shared_graph_state_schema *) (ctx->buffer + schema_offset);
	schema->class_name_offset = class_name_offset;
	schema->shape_offset = shape_offset;
	schema->count = (uint32_t) arr->nNumOfElements;

	zend_string_release(shape_key);

	ZVAL_LONG(&cached_offset, (zend_long) schema_offset);
	if (zend_hash_add(&ctx->state_schema_dedup, schema_key, &cached_offset) == NULL) {
		zend_string_release(schema_key);

		return false;
	}

	*offset = schema_offset;
	zend_string_release(schema_key);

	return true;
}

static bool user_cache_shared_graph_copy_shaped_state_values(
	php_user_cache_shared_graph_copy_context *ctx,
	const HashTable *state_array,
	uint32_t values_offset
)
{
	php_user_cache_shared_graph_value *graph_vals;
	zval *elem;
	uint32_t i;

	graph_vals = (php_user_cache_shared_graph_value *) (ctx->buffer + values_offset);
	i = 0;

	ZEND_HASH_FOREACH_VAL((HashTable *) state_array, elem) {
		if (!user_cache_shared_graph_copy_value(ctx, elem, &graph_vals[i])) {
			return false;
		}

		++i;
	} ZEND_HASH_FOREACH_END();

	return i == state_array->nNumOfElements;
}

static bool user_cache_shared_graph_copy_verbatim_value(
		php_user_cache_shared_graph_copy_context *ctx,
		const zval *src,
		zval *dst)
{
	const zval *src_packed;
	const HashTable *src_arr;
	const Bucket *src_bucket;
	zend_ulong arr_key;
	zend_array *target;
	zval *dst_packed, *cached, cached_offset;
	Bucket *dst_bucket;
	uint32_t i, string_offset, array_offset, data_offset, key_offset;
	size_t data_size;
	bool result = true;

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
			ZVAL_UNDEF(dst);
			return true;
		case IS_NULL:
			ZVAL_NULL(dst);
			return true;
		case IS_TRUE:
			ZVAL_TRUE(dst);
			return true;
		case IS_FALSE:
			ZVAL_FALSE(dst);
			return true;
		case IS_LONG:
			ZVAL_LONG(dst, Z_LVAL_P(src));
			return true;
		case IS_DOUBLE:
			ZVAL_DOUBLE(dst, Z_DVAL_P(src));
			return true;
		case IS_STRING:
			if (!user_cache_shared_graph_copy_string(ctx, Z_STR_P(src), &string_offset)) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, (zend_string *) (void *) (ctx->buffer + string_offset));

			/* Stack roots do not need relocation fixups. */
			if (user_cache_shared_graph_pointer_in_range(dst, ctx->buffer, ctx->size)) {
				user_cache_shared_graph_copy_record_fixup(ctx, dst);
			}

			return true;
		case IS_ARRAY:
			src_arr = Z_ARRVAL_P(src);
			if (src_arr->nNumOfElements == 0) {
				ZVAL_EMPTY_ARRAY(dst);

				return true;
			}

			if (GC_REFCOUNT(src_arr) > 1) {
				cached = zend_hash_index_find(&ctx->direct_array_dedup, (zend_ulong) (uintptr_t) src_arr);
				if (cached != NULL) {
					ZVAL_ARR(dst, (zend_array *) (void *) (ctx->buffer + (uint32_t) Z_LVAL_P(cached)));
					Z_TYPE_FLAGS_P(dst) = 0;

					if (user_cache_shared_graph_pointer_in_range(dst, ctx->buffer, ctx->size)) {
						user_cache_shared_graph_copy_record_fixup(ctx, dst);
					}

					return true;
				}
			}

			if (!php_user_cache_seen_test_and_add(&ctx->seen_arrays, src_arr)) {
				return false;
			}

			arr_key = (zend_ulong) (uintptr_t) src_arr;
			data_size = HT_IS_PACKED(src_arr) ? HT_PACKED_USED_SIZE(src_arr) : HT_USED_SIZE(src_arr);
			if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(zend_array), &array_offset) ||
				!user_cache_shared_graph_copy_alloc(ctx, data_size, &data_offset)
			) {
				result = false;

				goto done;
			}

			target = (zend_array *) (ctx->buffer + array_offset);

			memcpy(target, src_arr, sizeof(zend_array));
			memcpy(ctx->buffer + data_offset, HT_GET_DATA_ADDR(src_arr), data_size);

			GC_SET_REFCOUNT(target, 2);
			GC_TYPE_INFO(target) = GC_ARRAY | ((IS_ARRAY_IMMUTABLE | GC_NOT_COLLECTABLE) << GC_FLAGS_SHIFT);

			HT_FLAGS(target) |= HASH_FLAG_STATIC_KEYS;
			HT_SET_ITERATORS_COUNT(target, 0);

			target->pDestructor = NULL;
			target->nInternalPointer = 0;

			HT_SET_DATA_ADDR(target, ctx->buffer + data_offset);

			/* arData moves with the containing buffer. */
			user_cache_shared_graph_copy_record_fixup(ctx, &target->arData);

			if (HT_IS_PACKED(src_arr)) {
				dst_packed = target->arPacked;
				for (i = 0; i < src_arr->nNumUsed; i++) {
					src_packed = &src_arr->arPacked[i];
					if (!user_cache_shared_graph_copy_verbatim_value(ctx, src_packed, &dst_packed[i])) {
						result = false;

						break;
					}
				}
			} else {
				src_bucket = src_arr->arData;
				dst_bucket = target->arData;
				for (i = 0; i < src_arr->nNumUsed; i++) {
					if (src_bucket[i].key != NULL) {
						if (!user_cache_shared_graph_copy_string(
								ctx,
								src_bucket[i].key,
								&key_offset
							)
						) {
							result = false;

							break;
						}

						dst_bucket[i].key = (zend_string *) (void *) (ctx->buffer + key_offset);

						user_cache_shared_graph_copy_record_fixup(ctx, &dst_bucket[i].key);
					} else {
						dst_bucket[i].key = NULL;
					}

					if (!user_cache_shared_graph_copy_verbatim_value(ctx, &src_bucket[i].val, &dst_bucket[i].val)) {
						result = false;

						break;
					}
				}
			}

done:
			zend_hash_index_del(&ctx->seen_arrays, arr_key);

			if (!result) {
				return false;
			}

			if (GC_REFCOUNT(src_arr) > 1) {
				ZVAL_LONG(&cached_offset, (zend_long) array_offset);
				zend_hash_index_add(&ctx->direct_array_dedup, arr_key, &cached_offset);
			}

			ZVAL_ARR(dst, (zend_array *) (void *) (ctx->buffer + array_offset));
			Z_TYPE_FLAGS_P(dst) = 0;

			if (user_cache_shared_graph_pointer_in_range(dst, ctx->buffer, ctx->size)) {
				user_cache_shared_graph_copy_record_fixup(ctx, dst);
			}

			return true;
		default:
			return false;
	}
}

static bool user_cache_shared_graph_copy_shaped_state_object(
	php_user_cache_shared_graph_copy_context *ctx,
	zend_object *obj,
	const HashTable *state_array,
	uint8_t node_type,
	php_user_cache_shared_graph_value *dst
)
{
	php_user_cache_shared_graph_shaped_state_object *graph_shaped_state;
	uint32_t shaped_state_offset, state_schema_offset, state_values_offset;

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_user_cache_shared_graph_shaped_state_object),
			&shaped_state_offset
		) ||
		!user_cache_shared_graph_copy_state_schema(
			ctx,
			obj->ce,
			state_array,
			&state_schema_offset
		) ||
		!user_cache_shared_graph_copy_alloc(
			ctx,
			(size_t) state_array->nNumOfElements * sizeof(php_user_cache_shared_graph_value),
			&state_values_offset
		)
	) {
		return false;
	}

	if (!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			shaped_state_offset,
			shaped_state_offset + offsetof(php_user_cache_shared_graph_shaped_state_object, reserved)
		)
	) {
		return false;
	}

	graph_shaped_state = (php_user_cache_shared_graph_shaped_state_object *)
		(ctx->buffer + shaped_state_offset)
	;
	graph_shaped_state->state_schema_offset = state_schema_offset;
	graph_shaped_state->reserved = 0;
	graph_shaped_state->state_values_offset = state_values_offset;
	graph_shaped_state->state_next_free = (uint32_t) state_array->nNextFreeElement;

	if (!user_cache_shared_graph_copy_shaped_state_values(
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

static bool user_cache_shared_graph_copy_magic_state_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	zend_object *obj,
	php_user_cache_object_route route,
	php_user_cache_shared_graph_value *dst)
{
	php_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	php_user_cache_copy_object_ref_result ref_result;
	php_user_cache_shared_graph_state_getter_t get_state;
	zval *sd_state_ptr, sd_state;
	uint32_t sd_offset, sd_class_offset;
	bool result;

	ctx->has_userland_restore_object = true;

	ref_result = user_cache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst);
	if (ref_result != PHP_USER_CACHE_COPY_OBJECT_REF_NEW) {
		return ref_result == PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
	}

	get_state = user_cache_shared_graph_route_state_getter(route);
	if (!get_state(src, ctx->state_memo, &sd_state_ptr, &sd_state)) {
		return false;
	}

	if (user_cache_shared_graph_state_array_fits_schema(Z_ARRVAL_P(sd_state_ptr))) {
		result = user_cache_shared_graph_copy_shaped_state_object(
			ctx,
			obj,
			Z_ARRVAL_P(sd_state_ptr),
			PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT,
			dst
		);

		goto cleanup;
	}

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_user_cache_shared_graph_safe_direct_object), &sd_offset
		) ||
		!user_cache_shared_graph_copy_string(ctx, obj->ce->name, &sd_class_offset)
	) {
		result = false;

		goto cleanup;
	}

	if (!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(php_user_cache_shared_graph_safe_direct_object, reserved)
		)
	) {
		result = false;

		goto cleanup;
	}

	graph_safe_direct = (php_user_cache_shared_graph_safe_direct_object *) (ctx->buffer + sd_offset);
	graph_safe_direct->class_name_offset = sd_class_offset;
	graph_safe_direct->property_count = 0;
	graph_safe_direct->properties_offset = 0;
	graph_safe_direct->reserved = 0;

	if (!user_cache_shared_graph_copy_value(ctx, sd_state_ptr, &graph_safe_direct->state)) {
		result = false;

		goto cleanup;
	}

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT;
	dst->payload.offset = sd_offset;
	result = true;

cleanup:
	if (!Z_ISUNDEF(sd_state)) {
		zval_ptr_dtor(&sd_state);
	}

	return result;
}

static bool user_cache_shared_graph_copy_sleep_state_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	zend_object *obj,
	php_user_cache_object_route route,
	php_user_cache_shared_graph_value *dst)
{
	php_user_cache_shared_graph_object *graph_obj;
	php_user_cache_shared_graph_property *graph_properties;
	php_user_cache_copy_object_ref_result ref_result;
	php_user_cache_shared_graph_state_getter_t get_state;
	zend_ulong num_key;
	zend_string *prop_name, *resolved_name;
	zval *state_ptr, state, *prop_val;
	HashTable *props;
	uint32_t obj_offset, class_name_offset, properties_offset,
		prop_idx, prop_count
	;
	bool result;

	/* Values requiring restore hooks cannot use request-local cloning. */
	if (route != PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS ||
		zend_hash_find_known_hash(&obj->ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP)) != NULL
	) {
		ctx->has_userland_restore_object = true;
	}

	ref_result = user_cache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst);
	if (ref_result != PHP_USER_CACHE_COPY_OBJECT_REF_NEW) {
		return ref_result == PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
	}

	get_state = user_cache_shared_graph_route_state_getter(route);
	if (!get_state(src, ctx->state_memo, &state_ptr, &state)) {
		return false;
	}

	props = Z_ARRVAL_P(state_ptr);
	if (user_cache_shared_graph_state_array_fits_schema(props)) {
		result = user_cache_shared_graph_copy_shaped_state_object(
			ctx,
			obj,
			props,
			PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT,
			dst
		);

		goto cleanup;
	}

	prop_count = zend_hash_num_elements(props);
	properties_offset = 0;

	if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*graph_obj), &obj_offset) ||
		!user_cache_shared_graph_copy_string(ctx, obj->ce->name, &class_name_offset) ||
		(prop_count != 0 &&
			!user_cache_shared_graph_copy_alloc(
				ctx,
				(size_t) prop_count * sizeof(php_user_cache_shared_graph_property),
				&properties_offset
			)
		)
	) {
		result = false;

		goto cleanup;
	}

	if (!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(php_user_cache_shared_graph_object, reserved)
		)
	) {
		result = false;

		goto cleanup;
	}

	graph_obj = (php_user_cache_shared_graph_object *) (ctx->buffer + obj_offset);
	graph_obj->class_name_offset = class_name_offset;
	graph_obj->property_count = prop_count;
	graph_obj->properties_offset = properties_offset;
	graph_obj->reserved = 0;

	if (prop_count == 0) {
		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT;
		dst->payload.offset = obj_offset;
		result = true;

		goto cleanup;
	}

	graph_properties = (php_user_cache_shared_graph_property *) (ctx->buffer + properties_offset);
	prop_idx = 0;
	result = true;

	/* Native serialization restores integer keys as property names. */
	ZEND_HASH_FOREACH_KEY_VAL(props, num_key, prop_name, prop_val) {
		resolved_name = prop_name != NULL
			? zend_string_copy(prop_name)
			: zend_long_to_str((zend_long) num_key)
		;

		graph_properties[prop_idx].name_offset = 0;
		graph_properties[prop_idx].reserved =
			php_user_cache_serdes_declared_property_index_plus_one(obj->ce, resolved_name)
		;
		graph_properties[prop_idx].value.type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

		if (!user_cache_shared_graph_copy_string(
				ctx,
				resolved_name,
				&graph_properties[prop_idx].name_offset
			) ||
			!user_cache_shared_graph_copy_value(
				ctx,
				prop_val,
				&graph_properties[prop_idx].value
			)
		) {
			zend_string_release(resolved_name);
			result = false;

			break;
		}

		zend_string_release(resolved_name);
		++prop_idx;
	} ZEND_HASH_FOREACH_END();

	if (result) {
		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT;
		dst->payload.offset = obj_offset;
	}

cleanup:
	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	return result;
}

static bool user_cache_shared_graph_copy_safe_direct_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	zend_object *obj,
	php_user_cache_shared_graph_value *dst)
{
	const HashTable *sd_state_ht;
	php_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	php_user_cache_shared_graph_property *graph_properties;
	php_user_cache_copy_object_ref_result ref_result;
	zend_string *prop_name;
	zval *prop_val, *src_val, *sd_state_ptr, sd_state;
	HashTable *props;
	uint32_t string_offset, prop_idx, prop_count,
		sd_offset, sd_class_offset,
		sd_props_offset
	;
	bool result;

	if (php_user_cache_safe_direct_prefers_request_local_prototype(obj->ce)) {
		ctx->prefers_prototype = true;
	}

	ref_result = user_cache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst);
	if (ref_result != PHP_USER_CACHE_COPY_OBJECT_REF_NEW) {
		return ref_result == PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
	}

	if (!user_cache_shared_graph_get_safe_direct_state(
			src,
			ctx->state_memo,
			&sd_state_ptr,
			&sd_state
		)
	) {
		return false;
	}

	/* sd_state_ptr may dangle once recursion resizes the memo; keep the array. */
	sd_state_ht = Z_TYPE_P(sd_state_ptr) == IS_ARRAY ? Z_ARRVAL_P(sd_state_ptr) : NULL;

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_user_cache_shared_graph_safe_direct_object),
			&sd_offset
		) ||
		!user_cache_shared_graph_copy_string(
			ctx,
			obj->ce->name,
			&sd_class_offset
		)
	) {
		result = false;

		goto cleanup;
	}

	if (!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			sd_offset,
			sd_offset + offsetof(php_user_cache_shared_graph_safe_direct_object, reserved)
		)
	) {
		result = false;

		goto cleanup;
	}

	graph_safe_direct = (php_user_cache_shared_graph_safe_direct_object *) (ctx->buffer + sd_offset);
	graph_safe_direct->class_name_offset = sd_class_offset;
	graph_safe_direct->reserved = 0;

	if (!user_cache_shared_graph_copy_value(ctx, sd_state_ptr, &graph_safe_direct->state)) {
		result = false;

		goto cleanup;
	}

	props = zend_std_get_properties(obj);
	prop_count = props != NULL ? props->nNumOfElements : 0;
	graph_safe_direct->property_count = prop_count;
	graph_safe_direct->properties_offset = 0;
	result = true;

	if (prop_count != 0) {
		if (!user_cache_shared_graph_copy_alloc(
				ctx,
				(size_t) prop_count * sizeof(php_user_cache_shared_graph_property),
				&sd_props_offset
			)
		) {
			result = false;

			goto cleanup;
		}

		graph_safe_direct->properties_offset = sd_props_offset;
		graph_properties = (php_user_cache_shared_graph_property *) (ctx->buffer + sd_props_offset);
		prop_idx = 0;

		ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
			/* Zero the skipped slot; copy_alloc() only clears alignment padding. */
			memset(&graph_properties[prop_idx], 0, sizeof(graph_properties[prop_idx]));

			if (prop_name == NULL) {
				result = false;

				break;
			}

			src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
				? Z_INDIRECT_P(prop_val)
				: prop_val
			;

			if (user_cache_shared_graph_safe_direct_property_shadows_state(
					prop_name,
					sd_state_ht
				)
			) {
				++prop_idx;

				continue;
			}

			if (!user_cache_shared_graph_copy_string(ctx, prop_name, &string_offset)) {
				result = false;

				break;
			}

			graph_properties[prop_idx].name_offset = string_offset;

			if (!user_cache_shared_graph_copy_value(
					ctx,
					src_val,
					&graph_properties[prop_idx].value
				)
			) {
				result = false;

				break;
			}

			++prop_idx;
		} ZEND_HASH_FOREACH_END();

		if (!result) {
			goto cleanup;
		}
	}

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT;
	dst->payload.offset = sd_offset;

cleanup:
	if (!Z_ISUNDEF(sd_state)) {
		zval_ptr_dtor(&sd_state);
	}

	return result;
}

static bool user_cache_shared_graph_copy_serdes_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	zend_object *obj,
	php_user_cache_shared_graph_value *dst)
{
	php_user_cache_shared_graph_serdes_object *graph_serdes;
	php_user_cache_copy_object_ref_result ref_result;
	zend_string *serdes_blob;
	uint32_t serdes_offset;

	ctx->has_userland_restore_object = true;

	ref_result = user_cache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst);
	if (ref_result != PHP_USER_CACHE_COPY_OBJECT_REF_NEW) {
		return ref_result == PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
	}

	if (!user_cache_shared_graph_get_serdes_blob(src, ctx->state_memo, &serdes_blob)) {
		return false;
	}

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_user_cache_shared_graph_serdes_object) + ZSTR_LEN(serdes_blob),
			&serdes_offset
		)
	) {
		return false;
	}

	if (!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			serdes_offset,
			serdes_offset + offsetof(php_user_cache_shared_graph_serdes_object, flags)
		)
	) {
		return false;
	}

	graph_serdes = (php_user_cache_shared_graph_serdes_object *) (ctx->buffer + serdes_offset);
	graph_serdes->blob_len = (uint32_t) ZSTR_LEN(serdes_blob);
	graph_serdes->flags = 0;

	memcpy(graph_serdes + 1, ZSTR_VAL(serdes_blob), ZSTR_LEN(serdes_blob));

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT;
	dst->payload.offset = serdes_offset;

	return true;
}

static bool user_cache_shared_graph_copy_plain_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	zend_object *obj,
	php_user_cache_shared_graph_value *dst)
{
	php_user_cache_shared_graph_object *graph_obj;
	php_user_cache_shared_graph_property *graph_properties;
	php_user_cache_copy_object_ref_result ref_result;
	zend_string *prop_name;
	zval *prop_val, *src_val;
	HashTable *props;
	uint32_t obj_offset, class_name_offset, properties_offset,
		prop_idx, prop_count
	;

	ctx->prefers_prototype = true;

	ref_result = user_cache_shared_graph_copy_emit_object_ref_if_seen(ctx, obj, dst);
	if (ref_result != PHP_USER_CACHE_COPY_OBJECT_REF_NEW) {
		return ref_result == PHP_USER_CACHE_COPY_OBJECT_REF_EMITTED;
	}

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(*graph_obj),
			&obj_offset
		) ||
		!user_cache_shared_graph_seen_record_object_offsets(
			ctx,
			obj,
			obj_offset,
			obj_offset + offsetof(php_user_cache_shared_graph_object, reserved)
		) ||
		!user_cache_shared_graph_copy_string(
			ctx,
			obj->ce->name,
			&class_name_offset
		)
	) {
		return false;
	}

	props = zend_get_properties_for((zval *) src, ZEND_PROP_PURPOSE_SERIALIZE);
	prop_count = props != NULL ? props->nNumOfElements : 0;
	properties_offset = 0;
	if (prop_count != 0 &&
		!user_cache_shared_graph_copy_alloc(
			ctx,
			((size_t) prop_count * sizeof(php_user_cache_shared_graph_property)),
			&properties_offset
		)
	) {
		if (props != NULL) {
			zend_release_properties(props);
		}

		return false;
	}

	graph_obj = (php_user_cache_shared_graph_object *) (ctx->buffer + obj_offset);
	graph_obj->class_name_offset = class_name_offset;
	graph_obj->property_count = prop_count;
	graph_obj->properties_offset = properties_offset;
	graph_obj->reserved = 0;

	if (prop_count == 0) {
		if (props != NULL) {
			zend_release_properties(props);
		}

		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;
		dst->payload.offset = obj_offset;

		return true;
	}

	graph_properties = (php_user_cache_shared_graph_property *) (ctx->buffer + properties_offset);
	prop_idx = 0;

	ZEND_HASH_FOREACH_STR_KEY_VAL(props, prop_name, prop_val) {
		if (prop_name == NULL) {
			zend_release_properties(props);

			return false;
		}

		graph_properties[prop_idx].name_offset = 0;
		graph_properties[prop_idx].reserved = 0;
		graph_properties[prop_idx].value.type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

		src_val = Z_TYPE_P(prop_val) == IS_INDIRECT
			? Z_INDIRECT_P(prop_val)
			: prop_val
		;

		if (!user_cache_shared_graph_copy_string(ctx, prop_name, &graph_properties[prop_idx].name_offset)) {
			zend_release_properties(props);

			return false;
		}

		if (!user_cache_shared_graph_copy_value(
				ctx,
				src_val,
				&graph_properties[prop_idx].value
			)
		) {
			zend_release_properties(props);

			return false;
		}

		++prop_idx;
	} ZEND_HASH_FOREACH_END();

	zend_release_properties(props);

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;
	dst->payload.offset = obj_offset;

	return true;
}

static bool user_cache_shared_graph_copy_enum(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
)
{
	php_user_cache_shared_graph_enum *graph_enum;
	zend_class_entry *ce;
	zend_string *case_name;
	uint32_t enum_offset, enum_class_offset, enum_case_offset;

	ce = Z_OBJCE_P(src);
	case_name = Z_STR_P(zend_enum_fetch_case_name(Z_OBJ_P(src)));

	if (!user_cache_shared_graph_copy_alloc(ctx,
		sizeof(php_user_cache_shared_graph_enum), &enum_offset) ||
		!user_cache_shared_graph_copy_string(ctx, ce->name, &enum_class_offset) ||
		!user_cache_shared_graph_copy_string(ctx, case_name, &enum_case_offset)
	) {
		return false;
	}

	graph_enum = (php_user_cache_shared_graph_enum *) (ctx->buffer + enum_offset);
	graph_enum->class_name_offset = enum_class_offset;
	graph_enum->case_name_offset = enum_case_offset;

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM;
	dst->payload.offset = enum_offset;

	return true;
}

static bool user_cache_shared_graph_copy_reference(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
)
{
	php_user_cache_shared_graph_reference *graph_reference;
	zend_reference *ref;
	uint32_t shared_offset, reference_offset;
	void *seen_offset;

	ref = Z_REF_P(src);
	seen_offset = zend_hash_index_find_ptr(&ctx->seen_references, (zend_ulong) (uintptr_t) ref);

	if (seen_offset != NULL) {
		shared_offset = (uint32_t) (uintptr_t) seen_offset;
		((php_user_cache_shared_graph_reference *) (ctx->buffer + shared_offset))->flags |=
			PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
		;

		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF;
		dst->payload.offset = shared_offset;
		ctx->has_shared_identity = true;

		return true;
	}

	if (!user_cache_shared_graph_copy_alloc(
			ctx,
			sizeof(php_user_cache_shared_graph_reference),
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

	graph_reference = (php_user_cache_shared_graph_reference *) (ctx->buffer + reference_offset);
	graph_reference->flags = 0;
	graph_reference->reserved = 0;

	if (!user_cache_shared_graph_copy_value(
			ctx,
			&ref->val,
			&graph_reference->inner
		)
	) {
		return false;
	}

	dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE;
	dst->payload.offset = reference_offset;
	ctx->has_shared_identity = true;

	return true;
}

static bool user_cache_shared_graph_copy_array(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
)
{
	php_user_cache_shared_graph_array *graph_array;
	php_user_cache_shared_graph_array_element *graph_elems, *graph_elem;
	php_user_cache_shared_graph_shaped_array *graph_shaped_array;
	php_user_cache_shared_graph_value *graph_vals;
	zend_ulong arr_key, h;
	zend_string *key;
	zval *elem, *verdict, array_value;
	uint32_t elem_idx,
		array_offset, elems_offset, key_offset,
		shape_offset, values_offset,
		shared_offset
	;
	bool result, verbatim;
	void *seen_offset;

	result = true;

	if (Z_ARRVAL_P(src)->nNumOfElements == 0) {
		if (Z_ARRVAL_P(src)->nNextFreeElement == 0) {
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY;
			dst->payload.offset = 0;

			return true;
		}

		if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*graph_array), &array_offset)) {
			return false;
		}

		graph_array = (php_user_cache_shared_graph_array *) (ctx->buffer + array_offset);
		graph_array->count = 0;
		graph_array->next_free = (uint32_t) Z_ARRVAL_P(src)->nNextFreeElement;
		graph_array->elements_offset = 0;
		graph_array->reserved = 0;

		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
		dst->payload.offset = array_offset;

		return true;
	}

	if (ctx->verbatim_arrays_allowed) {
		if (GC_FLAGS(Z_ARRVAL_P(src)) & IS_ARRAY_IMMUTABLE) {
			verbatim = true;
		} else {
			/* No state hook ran, so address-keyed verdicts are still valid. */
			verdict = ctx->shared_verdicts != NULL
				? zend_hash_index_find(ctx->shared_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(src))
				: NULL;
			if (verdict != NULL) {
				verbatim = Z_TYPE_P(verdict) == IS_TRUE;
			} else {
				verbatim = user_cache_shared_graph_can_copy_verbatim_value(
					&ctx->seen_arrays,
					&ctx->direct_verdicts,
					src
				);
			}
		}

		if (verbatim) {
			if (!user_cache_shared_graph_copy_verbatim_value(ctx, src, &array_value)) {
				return false;
			}

			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY;
			dst->payload.offset = (uint32_t) ((uint8_t *) Z_ARRVAL(array_value) - ctx->buffer);

			/* Verbatim arrays require relocation fixups. */
			ctx->has_verbatim_array = true;

			return true;
		}
	}

	arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);

	seen_offset = zend_hash_index_find_ptr(&ctx->seen_arrays, arr_key);
	if (seen_offset != NULL) {
		shared_offset = (uint32_t) (uintptr_t) seen_offset;
		((php_user_cache_shared_graph_array *) (ctx->buffer + shared_offset))->reserved |=
			PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED
		;

		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF;
		dst->payload.offset = shared_offset;
		ctx->has_shared_identity = true;

		return true;
	}

	if (user_cache_shared_graph_array_has_shape(Z_ARRVAL_P(src))) {
		if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*graph_shaped_array), &array_offset) ||
			!user_cache_shared_graph_copy_alloc(
				ctx,
				(size_t) Z_ARRVAL_P(src)->nNumOfElements * sizeof(*graph_vals),
				&values_offset
			) ||
			!user_cache_shared_graph_copy_array_shape(ctx, Z_ARRVAL_P(src), &shape_offset, NULL)
		) {
			result = false;

			goto done;
		}

		graph_shaped_array = (php_user_cache_shared_graph_shaped_array *) (ctx->buffer + array_offset);
		graph_shaped_array->count = Z_ARRVAL_P(src)->nNumOfElements;
		graph_shaped_array->next_free = (uint32_t) Z_ARRVAL_P(src)->nNextFreeElement;
		graph_shaped_array->shape_offset = shape_offset;
		graph_shaped_array->reserved = 0;
		graph_shaped_array->values_offset = values_offset;
		graph_shaped_array->reserved2 = 0;

		if (zend_hash_index_add_ptr(&ctx->seen_arrays, arr_key, (void *) (uintptr_t) array_offset) == NULL) {
			result = false;

			goto done;
		}

		graph_vals = (php_user_cache_shared_graph_value *) (ctx->buffer + values_offset);
		elem_idx = 0;

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(src), elem) {
			if (!user_cache_shared_graph_copy_value(ctx, elem, &graph_vals[elem_idx])) {
				result = false;

				break;
			}

			++elem_idx;
		} ZEND_HASH_FOREACH_END();

		if (result) {
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY;
			dst->payload.offset = array_offset;
		}

		goto done;
	}

	if (!user_cache_shared_graph_copy_alloc(ctx, sizeof(*graph_array), &array_offset) ||
		!user_cache_shared_graph_copy_alloc(
			ctx,
			(size_t) Z_ARRVAL_P(src)->nNumOfElements * sizeof(*graph_elems),
			&elems_offset
		)
	) {
		result = false;

		goto done;
	}

	graph_array = (php_user_cache_shared_graph_array *) (ctx->buffer + array_offset);
	graph_array->count = Z_ARRVAL_P(src)->nNumOfElements;
	graph_array->next_free = (uint32_t) Z_ARRVAL_P(src)->nNextFreeElement;
	graph_array->elements_offset = elems_offset;
	graph_array->reserved =
		(HT_IS_PACKED(Z_ARRVAL_P(src)) && HT_IS_WITHOUT_HOLES(Z_ARRVAL_P(src))) ?
		PHP_USER_CACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED :
		0
	;

	if (zend_hash_index_add_ptr(&ctx->seen_arrays, arr_key, (void *) (uintptr_t) array_offset) == NULL) {
		result = false;

		goto done;
	}

	graph_elems = (php_user_cache_shared_graph_array_element *) (ctx->buffer + elems_offset);
	graph_elem = graph_elems;

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(src), h, key, elem) {
		memset(graph_elem, 0, sizeof(*graph_elem));
		graph_elem->h = h;

		if (key != NULL) {
			if (!user_cache_shared_graph_copy_string(ctx, key, &key_offset)) {
				result = false;

				break;
			}

			graph_elem->key_offset = key_offset;
		}

		if (!user_cache_shared_graph_copy_value(ctx, elem, &graph_elem->value)) {
			result = false;

			break;
		}

		++graph_elem;
	} ZEND_HASH_FOREACH_END();

	if (result) {
		dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY;
		dst->payload.offset = array_offset;
	}

done:
	zend_hash_index_del(&ctx->seen_arrays, arr_key);

	return result;
}

static bool user_cache_shared_graph_copy_object(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
)
{
	zend_class_entry *ce;
	zend_object *obj;

	ctx->has_object = true;
	ce = Z_OBJCE_P(src);

	if (ce->ce_flags & ZEND_ACC_ENUM) {
		return user_cache_shared_graph_copy_enum(ctx, src, dst);
	}

	obj = Z_OBJ_P(src);

	switch (user_cache_shared_graph_classify_object_route(obj->ce)) {
		case PHP_USER_CACHE_OBJECT_ROUTE_SAFE_DIRECT:
			return user_cache_shared_graph_copy_safe_direct_object(ctx, src, obj, dst);
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE:
			return user_cache_shared_graph_copy_magic_state_object(
				ctx, src, obj, PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_SERIALIZE, dst
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS:
			return user_cache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_USER_CACHE_OBJECT_ROUTE_SERIALIZE_PROPS, dst
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE:
			return user_cache_shared_graph_copy_magic_state_object(
				ctx, src, obj, PHP_USER_CACHE_OBJECT_ROUTE_MAGIC_UNSERIALIZE, dst
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SLEEP:
			return user_cache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_USER_CACHE_OBJECT_ROUTE_SLEEP, dst
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP:
			return user_cache_shared_graph_copy_sleep_state_object(
				ctx, src, obj, PHP_USER_CACHE_OBJECT_ROUTE_WAKEUP, dst
			);
		case PHP_USER_CACHE_OBJECT_ROUTE_SERDES:
			return user_cache_shared_graph_copy_serdes_object(ctx, src, obj, dst);
		case PHP_USER_CACHE_OBJECT_ROUTE_PLAIN:
			return user_cache_shared_graph_copy_plain_object(ctx, src, obj, dst);
		case PHP_USER_CACHE_OBJECT_ROUTE_UNSTORABLE:
			return false;
	}

	return false;
}

static bool user_cache_shared_graph_copy_value(
	php_user_cache_shared_graph_copy_context *ctx,
	const zval *src,
	php_user_cache_shared_graph_value *dst
)
{
	uint32_t string_offset;

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	memset(dst, 0, sizeof(*dst));

	switch (Z_TYPE_P(src)) {
		case IS_UNDEF:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF;

			return true;
		case IS_NULL:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_NULL;

			return true;
		case IS_TRUE:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_TRUE;

			return true;
		case IS_FALSE:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_FALSE;

			return true;
		case IS_LONG:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_LONG;
			dst->payload.long_value = Z_LVAL_P(src);

			return true;
		case IS_DOUBLE:
			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE;
			dst->payload.double_value = Z_DVAL_P(src);

			return true;
		case IS_STRING:
			if (!user_cache_shared_graph_copy_string(ctx, Z_STR_P(src), &string_offset)) {
				return false;
			}

			dst->type = PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING;
			dst->payload.offset = string_offset;

			return true;
		case IS_ARRAY:
			return user_cache_shared_graph_copy_array(ctx, src, dst);
		case IS_OBJECT:
			return user_cache_shared_graph_copy_object(ctx, src, dst);
		case IS_REFERENCE:
			return user_cache_shared_graph_copy_reference(ctx, src, dst);
		default:
			return false;
	}
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_try_update_declared_property(
	zend_object *obj,
	zend_string *prop_name,
	zend_property_info *prop_info,
	zval *prop_val,
	bool *failed
)
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

static PHP_USER_CACHE_DECODE_HOT zend_class_entry *user_cache_decode_lookup_class(zend_string *class_name)
{
	zend_class_entry *ce;

	ce = user_cache_decode_resolve_cache_find(class_name);
	if (ce != NULL) {
		return ce;
	}

	ce = zend_lookup_class(class_name);
	if (ce != NULL) {
		user_cache_decode_resolve_cache_store(class_name, ce);
	}

	return ce;
}

static PHP_USER_CACHE_DECODE_HOT zend_class_entry *user_cache_decode_lookup_state_schema_class(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_state_schema *state_schema
)
{
	zend_string *class_name;
	zend_class_entry *ce;

	ce = user_cache_decode_resolve_cache_find(state_schema);
	if (ce != NULL) {
		return ce;
	}

	class_name = user_cache_decode_string_at(buf, buf_len, state_schema->class_name_offset);
	if (class_name == NULL) {
		return NULL;
	}

	ce = user_cache_decode_lookup_class(class_name);
	if (ce != NULL) {
		user_cache_decode_resolve_cache_store(state_schema, ce);
	}

	return ce;
}

static void user_cache_decode_array_dtor(zval *zv)
{
	zval tmp;

	ZVAL_ARR(&tmp, (zend_array *) Z_PTR_P(zv));

	zval_ptr_dtor(&tmp);
}

static void user_cache_decode_shape_prototype_dtor(zval *zv)
{
	zend_array_destroy((zend_array *) Z_PTR_P(zv));
}

static zend_array *user_cache_decode_shape_prototype_create(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_array_shape *graph_shape
)
{
	const php_user_cache_shared_graph_array_shape_element *shape_elems, *shape_elem;
	zend_string *prop_name;
	zend_array *proto;
	zval empty;
	uint32_t i;

	if (graph_shape->count == 0 ||
		graph_shape->count > PHP_USER_CACHE_SHARED_GRAPH_ARRAY_SHAPE_MAX_KEYS ||
		!user_cache_decode_array_range_ok(
			buf_len,
			graph_shape->elements_offset,
			graph_shape->count,
			sizeof(*shape_elems)
		)
	) {
		return NULL;
	}

	proto = zend_new_array(graph_shape->count);
	if (HT_FLAGS(proto) & HASH_FLAG_UNINITIALIZED) {
		zend_hash_real_init_mixed(proto);
	}

	shape_elems =
		(const php_user_cache_shared_graph_array_shape_element *) (buf + graph_shape->elements_offset)
	;

	ZVAL_LONG(&empty, 0);

	for (i = 0; i < graph_shape->count; i++) {
		shape_elem = &shape_elems[i];
		prop_name = user_cache_decode_string_at(buf, buf_len, shape_elem->key_offset);
		if (shape_elem->key_offset == 0 || prop_name == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}

		if (_zend_hash_append_ex(proto, prop_name, &empty, true) == NULL) {
			zend_array_destroy(proto);

			return NULL;
		}
	}

	proto->nNextFreeElement = 0;

	return proto;
}

static zend_always_inline void user_cache_decode_shape_prototype_direct_cache_store(
	const php_user_cache_shared_graph_array_shape *graph_shape,
	zend_array *proto
)
{
	uint32_t slot;

	slot = UC_G(decode_shape_prototype_direct_next)++ % PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS;

	UC_G(decode_shape_prototype_direct_keys)[slot] = graph_shape;
	UC_G(decode_shape_prototype_direct_values)[slot] = proto;
}

static zend_array *user_cache_decode_shape_prototype_get(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_array_shape *graph_shape
)
{
	zend_ulong key;
	zend_array *proto;
	HashTable *cache;
	uint32_t i;

	for (i = 0; i < PHP_USER_CACHE_DECODE_DIRECT_CACHE_SLOTS; i++) {
		if (UC_G(decode_shape_prototype_direct_keys)[i] == graph_shape) {
			return UC_G(decode_shape_prototype_direct_values)[i];
		}
	}

	key = (zend_ulong) (uintptr_t) graph_shape;
	if (UC_G(decode_shape_prototype_cache) != NULL) {
		proto = zend_hash_index_find_ptr(UC_G(decode_shape_prototype_cache), key);
		if (proto != NULL) {
			user_cache_decode_shape_prototype_direct_cache_store(graph_shape, proto);

			return proto;
		}
	}

	proto = user_cache_decode_shape_prototype_create(buf, buf_len, graph_shape);
	if (proto == NULL) {
		return NULL;
	}

	cache = user_cache_decode_shape_prototype_cache();
	if (zend_hash_index_add_ptr(cache, key, proto) == NULL) {
		zend_array_destroy(proto);

		return NULL;
	}

	user_cache_decode_shape_prototype_direct_cache_store(graph_shape, proto);

	return proto;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_decode_shape_prototype_clone(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_array_shape *graph_shape,
	uint32_t count,
	uint32_t next_free,
	zval *dst
)
{
	zend_array *proto, *arr;

	if (graph_shape->count != count) {
		return false;
	}

	proto = user_cache_decode_shape_prototype_get(buf, buf_len, graph_shape);
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

static zend_always_inline bool user_cache_shared_graph_decode_simple_value(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	zend_string *str;

	switch (value->type) {
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF:
			ZVAL_UNDEF(dst);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_NULL:
			ZVAL_NULL(dst);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_TRUE:
			ZVAL_TRUE(dst);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_FALSE:
			ZVAL_FALSE(dst);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_LONG:
			ZVAL_LONG(dst, value->payload.long_value);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE:
			ZVAL_DOUBLE(dst, value->payload.double_value);
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			str = user_cache_decode_string_at(buf, buf_len, (uint32_t) value->payload.offset);
			if (str == NULL) {
				return false;
			}

			ZVAL_INTERNED_STR(dst, str);

			return true;
		default:
			return false;
	}
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_shaped_array(
	const uint8_t *buf,
	size_t buf_len,
	uint32_t shape_offset,
	uint32_t values_offset,
	uint32_t count,
	uint32_t next_free,
	uint32_t node_offset,
	bool shared,
	zval *dst
)
{
	const php_user_cache_shared_graph_array_shape *graph_shape;
	const php_user_cache_shared_graph_value *graph_vals;
	zval val;
	Bucket *bucket;
	uint32_t i;

	if (!user_cache_decode_range_ok(buf_len, shape_offset, sizeof(*graph_shape)) ||
		!user_cache_decode_array_range_ok(buf_len, values_offset, count, sizeof(*graph_vals))
	) {
		return false;
	}

	graph_shape = (const php_user_cache_shared_graph_array_shape *) (buf + shape_offset);
	if (!user_cache_decode_shape_prototype_clone(
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
		if (!user_cache_decode_array_map_insert(node_offset, Z_ARRVAL_P(dst))) {
			return user_cache_decode_fail_zval(dst);
		}
	}

	HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	graph_vals = (const php_user_cache_shared_graph_value *) (buf + values_offset);
	bucket = Z_ARRVAL_P(dst)->arData;
	for (i = 0; i < count; i++) {
		if (user_cache_shared_graph_decode_simple_value(buf, buf_len, &graph_vals[i], &bucket[i].val)) {
			continue;
		}

		ZVAL_UNDEF(&val);
		if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_vals[i], &val)) {
			return user_cache_decode_fail_zval(dst);
		}

		ZVAL_COPY_VALUE(&bucket[i].val, &val);
	}

	PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_dynamic_array(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_array *graph_array;
	const php_user_cache_shared_graph_array_element *graph_elems, *graph_elem;
	zend_string *key;
	zval val;
	uint32_t i;

	if (!user_cache_decode_range_ok(buf_len, (uint32_t) value->payload.offset, sizeof(*graph_array))) {
		return false;
	}

	graph_array = (const php_user_cache_shared_graph_array *) (buf + (uint32_t) value->payload.offset);
	if (!user_cache_decode_array_range_ok(buf_len, graph_array->elements_offset, graph_array->count, sizeof(*graph_elems))) {
		return false;
	}

	array_init_size(dst, graph_array->count);

	/* Initialize before identity registration raises the array refcount. */
	if (graph_array->count > 0) {
		if (graph_array->reserved & PHP_USER_CACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED) {
			zend_hash_real_init_packed(Z_ARRVAL_P(dst));
		} else {
			zend_hash_real_init_mixed(Z_ARRVAL_P(dst));
		}

		HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	}

	if (graph_array->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) {
		if (!user_cache_decode_array_map_insert((uint32_t) value->payload.offset, Z_ARRVAL_P(dst))) {
			return user_cache_decode_fail_zval(dst);
		}
	}

	graph_elems = (const php_user_cache_shared_graph_array_element *) (buf + graph_array->elements_offset);

	if (graph_array->reserved & PHP_USER_CACHE_SHARED_GRAPH_ARRAY_FLAG_PACKED) {
		for (i = 0; i < graph_array->count; i++) {
			graph_elem = &graph_elems[i];

			if (graph_elem->key_offset != 0 || graph_elem->h != i) {
				return user_cache_decode_fail_zval(dst);
			}

			if (!user_cache_shared_graph_decode_simple_value(buf, buf_len, &graph_elem->value, &val)) {
				ZVAL_UNDEF(&val);

				if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_elem->value, &val)) {
					return user_cache_decode_fail_zval(dst);
				}
			}

			if (zend_hash_next_index_insert_new(Z_ARRVAL_P(dst), &val) == NULL) {
				zval_ptr_dtor(&val);
				return user_cache_decode_fail_zval(dst);
			}
		}

		Z_ARRVAL_P(dst)->nNextFreeElement = graph_array->next_free;
		PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
		return true;
	}

	for (i = 0; i < graph_array->count; i++) {
		graph_elem = &graph_elems[i];

		if (!user_cache_shared_graph_decode_simple_value(buf, buf_len, &graph_elem->value, &val)) {
			ZVAL_UNDEF(&val);

			if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_elem->value, &val)) {
				return user_cache_decode_fail_zval(dst);
			}
		}

		if (graph_elem->key_offset != 0) {
			key = user_cache_decode_string_at(buf, buf_len, graph_elem->key_offset);
			if (key == NULL) {
				zval_ptr_dtor(&val);
				return user_cache_decode_fail_zval(dst);
			}

			if (zend_hash_add_new(Z_ARRVAL_P(dst), key, &val) == NULL) {
				zval_ptr_dtor(&val);
				return user_cache_decode_fail_zval(dst);
			}
		} else {
			if (zend_hash_index_add_new(Z_ARRVAL_P(dst), graph_elem->h, &val) == NULL) {
				zval_ptr_dtor(&val);
				return user_cache_decode_fail_zval(dst);
			}
		}
	}

	Z_ARRVAL_P(dst)->nNextFreeElement = graph_array->next_free;
	PHP_USER_CACHE_HT_DISALLOW_COW_VIOLATION(Z_ARRVAL_P(dst));
	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_init_object(
	zend_class_entry *ce,
	uint32_t node_offset,
	bool shared,
	zval *dst
)
{
	if (object_init_ex(dst, ce) != SUCCESS) {
		return false;
	}

	if (shared && !user_cache_decode_identity_map_insert(node_offset, Z_OBJ_P(dst))) {
		return user_cache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_apply_property(
	zval *dst,
	zend_string *prop_name,
	uint32_t slot_idx_plus_one,
	zval *prop_val
)
{
	bool result;

	if (slot_idx_plus_one != 0) {
		result = php_user_cache_shared_graph_update_object_property_at(
			dst,
			prop_name,
			slot_idx_plus_one - 1,
			prop_val
		);
	} else {
		result = php_user_cache_shared_graph_update_object_property(
			dst,
			prop_name,
			prop_val
		);
	}

	if (!result) {
		zval_ptr_dtor(prop_val);
		return user_cache_decode_fail_zval(dst);
	}

	zval_ptr_dtor(prop_val);

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_object_properties(
	const uint8_t *buf,
	size_t buf_len,
	uint32_t properties_offset,
	uint32_t property_count,
	bool use_sleep_slots,
	zval *dst
)
{
	const php_user_cache_shared_graph_property *props, *prop;
	zend_string *prop_name;
	zval prop_val;
	uint32_t i;

	if (property_count == 0) {
		return true;
	}

	if (!user_cache_decode_array_range_ok(buf_len, properties_offset, property_count, sizeof(*props))) {
		return user_cache_decode_fail_zval(dst);
	}

	props = (const php_user_cache_shared_graph_property *) (buf + properties_offset);
	for (i = 0; i < property_count; i++) {
		prop = &props[i];
		if (prop->value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF) {
			continue;
		}

		prop_name = user_cache_decode_string_at(buf, buf_len, prop->name_offset);
		if (prop_name == NULL) {
			return user_cache_decode_fail_zval(dst);
		}

		if (!user_cache_shared_graph_decode_simple_value(buf, buf_len, &prop->value, &prop_val)) {
			ZVAL_UNDEF(&prop_val);

			if (!user_cache_shared_graph_decode_value(buf, buf_len, &prop->value, &prop_val)) {
				zval_ptr_dtor(&prop_val);
				return user_cache_decode_fail_zval(dst);
			}
		}

		if (!user_cache_shared_graph_decode_apply_property(
				dst,
				prop_name,
				use_sleep_slots ? prop->reserved : i + 1,
				&prop_val
			)
		) {
			return false;
		}
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_call_wakeup(zend_class_entry *ce, zval *dst)
{
	zval retval, *wakeup_zv;

	wakeup_zv = zend_hash_find_known_hash(&ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));
	if (wakeup_zv != NULL) {
		zend_call_known_instance_method(Z_FUNC_P(wakeup_zv), Z_OBJ_P(dst), &retval, 0, NULL);

		zval_ptr_dtor(&retval);

		if (EG(exception)) {
			return user_cache_decode_fail_zval(dst);
		}
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_call_unserialize(zend_class_entry *ce, zval *dst, zval *state)
{
	zend_call_known_instance_method_with_1_params(
		ce->__unserialize,
		Z_OBJ_P(dst),
		NULL,
		state
	);

	zval_ptr_dtor(state);

	if (EG(exception)) {
		return user_cache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_object *graph_obj;
	zend_string *class_name;
	zend_class_entry *ce;

	graph_obj = (const php_user_cache_shared_graph_object *) (buf + (uint32_t) value->payload.offset);
	class_name = user_cache_decode_string_at(buf, buf_len, graph_obj->class_name_offset);
	if (class_name == NULL) {
		return false;
	}
	ce = user_cache_decode_lookup_class(class_name);

	if (ce == NULL) {
		return false;
	}

	if (!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_obj->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	return user_cache_shared_graph_decode_object_properties(
		buf,
		buf_len,
		graph_obj->properties_offset,
		graph_obj->property_count,
		false,
		dst
	);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_sleep_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_object *graph_obj;
	zend_string *class_name;
	zend_class_entry *ce;

	graph_obj = (const php_user_cache_shared_graph_object *) (buf + (uint32_t) value->payload.offset);
	class_name = user_cache_decode_string_at(buf, buf_len, graph_obj->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = user_cache_decode_lookup_class(class_name);
	if (ce == NULL) {
		return false;
	}

	if (!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_obj->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!user_cache_shared_graph_decode_object_properties(
			buf,
			buf_len,
			graph_obj->properties_offset,
			graph_obj->property_count,
			true,
			dst
		)
	) {
		return false;
	}

	return user_cache_shared_graph_decode_call_wakeup(ce, dst);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_sleep_shaped_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_shaped_state_object *graph_shaped_state;
	const php_user_cache_shared_graph_state_schema *state_schema;
	const php_user_cache_shared_graph_array_shape *graph_shape;
	const php_user_cache_shared_graph_array_shape_element *shape_elems;
	const php_user_cache_shared_graph_value *graph_vals;
	zend_string *prop_name;
	zend_class_entry *ce;
	zval prop_val;
	uint32_t i, prop_idx_plus_one;

	graph_shaped_state = (const php_user_cache_shared_graph_shaped_state_object *) (buf + (uint32_t) value->payload.offset);
	if (!user_cache_decode_range_ok(buf_len, graph_shaped_state->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const php_user_cache_shared_graph_state_schema *) (buf + graph_shaped_state->state_schema_offset);
	ce = user_cache_decode_lookup_state_schema_class(buf, buf_len, state_schema);

	if (ce == NULL) {
		return false;
	}

	if (!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_shaped_state->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	if (!user_cache_decode_range_ok(buf_len, state_schema->shape_offset, sizeof(*graph_shape))) {
		return user_cache_decode_fail_zval(dst);
	}

	graph_shape = (const php_user_cache_shared_graph_array_shape *) (buf + state_schema->shape_offset);
	if (graph_shape->count != state_schema->count) {
		return user_cache_decode_fail_zval(dst);
	}

	if (!user_cache_decode_array_range_ok(buf_len, graph_shape->elements_offset, state_schema->count, sizeof(*shape_elems)) ||
		!user_cache_decode_array_range_ok(buf_len, graph_shaped_state->state_values_offset, state_schema->count, sizeof(*graph_vals))
	) {
		return user_cache_decode_fail_zval(dst);
	}

	shape_elems = (const php_user_cache_shared_graph_array_shape_element *) (buf + graph_shape->elements_offset);
	graph_vals = (const php_user_cache_shared_graph_value *) (buf + graph_shaped_state->state_values_offset);
	for (i = 0; i < state_schema->count; i++) {
		prop_name = user_cache_decode_string_at(buf, buf_len, shape_elems[i].key_offset);
		if (prop_name == NULL) {
			return user_cache_decode_fail_zval(dst);
		}

		if (!user_cache_shared_graph_decode_simple_value(buf, buf_len, &graph_vals[i], &prop_val)) {
			ZVAL_UNDEF(&prop_val);

			if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_vals[i], &prop_val)) {
				zval_ptr_dtor(&prop_val);
				return user_cache_decode_fail_zval(dst);
			}
		}

		prop_idx_plus_one = php_user_cache_serdes_declared_property_index_plus_one(ce, prop_name);
		if (!user_cache_shared_graph_decode_apply_property(
				dst,
				prop_name,
				prop_idx_plus_one,
				&prop_val
			)
		) {
			return false;
		}
	}

	return user_cache_shared_graph_decode_call_wakeup(ce, dst);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_safe_direct_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	zend_string *class_name;
	zend_class_entry *ce;
	php_user_cache_safe_direct_state_unserialize_func_t unserialize_func;
	zval sd_state;

	graph_safe_direct = (const php_user_cache_shared_graph_safe_direct_object *) (buf + (uint32_t) value->payload.offset);

	class_name = user_cache_decode_string_at(buf, buf_len, graph_safe_direct->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = user_cache_decode_lookup_class(class_name);
	if (ce == NULL) {
		return false;
	}

	unserialize_func = php_user_cache_safe_direct_state_unserialize_func(ce);

	if (unserialize_func == NULL ||
		!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_safe_direct->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&sd_state);
	if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_safe_direct->state, &sd_state) ||
		Z_TYPE(sd_state) != IS_ARRAY ||
		!unserialize_func(dst, &sd_state)
	) {
		zval_ptr_dtor(&sd_state);
		return user_cache_decode_fail_zval(dst);
	}

	zval_ptr_dtor(&sd_state);

	return user_cache_shared_graph_decode_object_properties(
		buf,
		buf_len,
		graph_safe_direct->properties_offset,
		graph_safe_direct->property_count,
		false,
		dst
	);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_serialized_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_safe_direct_object *graph_serialized;
	zend_string *class_name;
	zend_class_entry *ce;
	zval state;

	graph_serialized = (const php_user_cache_shared_graph_safe_direct_object *) (buf + (uint32_t) value->payload.offset);

	class_name = user_cache_decode_string_at(buf, buf_len, graph_serialized->class_name_offset);
	if (class_name == NULL) {
		return false;
	}

	ce = user_cache_decode_lookup_class(class_name);
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_serialized->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!user_cache_shared_graph_decode_value(buf, buf_len, &graph_serialized->state, &state) ||
		Z_TYPE(state) != IS_ARRAY
	) {
		zval_ptr_dtor(&state);
		return user_cache_decode_fail_zval(dst);
	}

	return user_cache_shared_graph_decode_call_unserialize(ce, dst, &state);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_serialized_shaped_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_shaped_state_object *graph_shaped_state;
	const php_user_cache_shared_graph_state_schema *state_schema;
	zend_class_entry *ce;
	zval state;

	graph_shaped_state = (const php_user_cache_shared_graph_shaped_state_object *) (buf + (uint32_t) value->payload.offset);
	if (!user_cache_decode_range_ok(buf_len, graph_shaped_state->state_schema_offset, sizeof(*state_schema))) {
		return false;
	}

	state_schema = (const php_user_cache_shared_graph_state_schema *) (buf + graph_shaped_state->state_schema_offset);
	ce = user_cache_decode_lookup_state_schema_class(buf, buf_len, state_schema);
	if (ce == NULL || ce->__unserialize == NULL || (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE)) {
		return false;
	}

	if (!user_cache_shared_graph_decode_init_object(
			ce,
			(uint32_t) value->payload.offset,
			(graph_shaped_state->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
			dst
		)
	) {
		return false;
	}

	ZVAL_UNDEF(&state);

	if (!user_cache_shared_graph_decode_shaped_array(
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
		return user_cache_decode_fail_zval(dst);
	}

	return user_cache_shared_graph_decode_call_unserialize(ce, dst, &state);
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_serdes_object(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_serdes_object *graph_serdes;

	graph_serdes =
		(const php_user_cache_shared_graph_serdes_object *) (buf + (uint32_t) value->payload.offset)
	;

	if (!user_cache_decode_range_ok(
			buf_len,
			(uint32_t) value->payload.offset + (uint32_t) sizeof(*graph_serdes),
			graph_serdes->blob_len)
	) {
		return false;
	}

	if (!php_user_cache_serdes_decode(
			(const uint8_t *) (graph_serdes + 1),
			graph_serdes->blob_len,
			dst
		) ||
		Z_TYPE_P(dst) != IS_OBJECT
	) {
		return user_cache_decode_fail_zval(dst);
	}

	if ((graph_serdes->flags & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
		!user_cache_decode_identity_map_insert(
			(uint32_t) value->payload.offset,
			Z_OBJ_P(dst)
		)
	) {
		return user_cache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_enum(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_enum *graph_enum;
	zend_string *class_name, *case_name;
	zend_class_entry *ce;
	zend_object *case_obj;

	graph_enum = (const php_user_cache_shared_graph_enum *) (buf + (uint32_t) value->payload.offset);
	case_obj = user_cache_decode_resolve_cache_find(graph_enum);
	if (case_obj == NULL) {
		class_name = user_cache_decode_string_at(buf, buf_len, graph_enum->class_name_offset);
		case_name = user_cache_decode_string_at(buf, buf_len, graph_enum->case_name_offset);
		if (class_name == NULL || case_name == NULL) {
			return false;
		}

		ce = user_cache_decode_lookup_class(class_name);

		if (ce == NULL || !(ce->ce_flags & ZEND_ACC_ENUM)) {
			return false;
		}

		case_obj = zend_enum_get_case(ce, case_name);
		if (case_obj == NULL) {
			return false;
		}

		user_cache_decode_resolve_cache_store(graph_enum, case_obj);
	}

	ZVAL_OBJ(dst, case_obj);
	GC_ADDREF(case_obj);

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_reference(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_reference *graph_ref;
	zend_reference *ref;

	graph_ref = (const php_user_cache_shared_graph_reference *) (buf + (uint32_t) value->payload.offset);

	ZVAL_NEW_EMPTY_REF(dst);
	ref = Z_REF_P(dst);

	ZVAL_UNDEF(&ref->val);

	if ((graph_ref->flags & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) &&
		!user_cache_decode_reference_map_insert(
			(uint32_t) value->payload.offset,
			ref
		)
	) {
		return user_cache_decode_fail_zval(dst);
	}

	if (!user_cache_shared_graph_decode_value(
			buf,
			buf_len,
			&graph_ref->inner,
			&ref->val
		)
	) {
		return user_cache_decode_fail_zval(dst);
	}

	return true;
}

static PHP_USER_CACHE_DECODE_HOT bool user_cache_shared_graph_decode_value(
	const uint8_t *buf,
	size_t buf_len,
	const php_user_cache_shared_graph_value *value,
	zval *dst
)
{
	const php_user_cache_shared_graph_shaped_array *graph_shaped_array;
	zend_reference *shared_reference;
	zend_array *shared_array;
	zend_object *shared_obj;
	size_t node_header_size;

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	/* Validate the node header before dereferencing it. */
	node_header_size = user_cache_decode_node_header_size(value->type);
	if (node_header_size != 0 &&
		!user_cache_decode_range_ok(buf_len, (uint32_t) value->payload.offset, node_header_size)
	) {
		return false;
	}

	switch (value->type) {
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_UNDEF:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_NULL:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_TRUE:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_FALSE:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_LONG:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DOUBLE:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			return user_cache_shared_graph_decode_simple_value(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				ZVAL_EMPTY_ARRAY(dst);
			} else {
				if (!user_cache_decode_range_ok(
					buf_len,
					(uint32_t) value->payload.offset,
					sizeof(zend_array))
				) {
					return false;
				}

				ZVAL_ARR(dst, (zend_array *) (void *) (buf + (uint32_t) value->payload.offset));
				Z_TYPE_FLAGS_P(dst) = 0;
			}

			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			return user_cache_shared_graph_decode_dynamic_array(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			graph_shaped_array = (const php_user_cache_shared_graph_shaped_array *) (buf + (uint32_t) value->payload.offset);

			return user_cache_shared_graph_decode_shaped_array(
				buf,
				buf_len,
				graph_shaped_array->shape_offset,
				graph_shaped_array->values_offset,
				graph_shaped_array->count,
				graph_shaped_array->next_free,
				(uint32_t) value->payload.offset,
				(graph_shaped_array->reserved & PHP_USER_CACHE_SHARED_GRAPH_OBJECT_FLAG_SHARED) != 0,
				dst
			);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY_REF: {
			shared_array = user_cache_decode_array_map_find((uint32_t) value->payload.offset);
			if (shared_array == NULL) {
				return false;
			}

			ZVAL_ARR(dst, shared_array);
			GC_ADDREF(shared_array);

			return true;
		}
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
			return user_cache_shared_graph_decode_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			return user_cache_shared_graph_decode_sleep_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
			return user_cache_shared_graph_decode_sleep_shaped_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT_REF: {
			shared_obj = user_cache_decode_identity_map_find((uint32_t) value->payload.offset);
			if (shared_obj == NULL) {
				return false;
			}

			ZVAL_OBJ(dst, shared_obj);
			GC_ADDREF(shared_obj);

			return true;
		}
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
			return user_cache_shared_graph_decode_safe_direct_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			return user_cache_shared_graph_decode_serialized_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			return user_cache_shared_graph_decode_serialized_shaped_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return user_cache_shared_graph_decode_serdes_object(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM:
			return user_cache_shared_graph_decode_enum(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return user_cache_shared_graph_decode_reference(buf, buf_len, value, dst);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE_REF: {
			shared_reference = user_cache_decode_reference_map_find((uint32_t) value->payload.offset);

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

static php_user_cache_shared_graph_header *user_cache_shared_graph_payload_header(uint32_t payload_offset)
{
	const uint8_t *graph_buf;
	php_user_cache_shared_graph_header *header;
	size_t buf_len;

	if (payload_offset == 0) {
		return NULL;
	}

	buf_len = php_user_cache_block_payload_capacity(payload_offset);
	if (buf_len == 0) {
		return NULL;
	}

	graph_buf = user_cache_shared_graph_locate(
		php_user_cache_ptr(payload_offset),
		buf_len,
		NULL
	);
	if (graph_buf == NULL) {
		return NULL;
	}

	header = (php_user_cache_shared_graph_header *) graph_buf;

	return header;
}

/* Debug-only cross-check for recorded relocation fixups. */
#if ZEND_DEBUG
static bool user_cache_shared_graph_rebase_verbatim_zval(
		php_user_cache_shared_graph_rebase_context *ctx,
		zval *value)
{
	zend_array *arr;

	switch (Z_TYPE_P(value)) {
		case IS_STRING:
			Z_STR_P(value) = (zend_string *) user_cache_shared_graph_rebase_pointer(
				Z_STR_P(value),
				ctx->old_base,
				ctx->len,
				ctx->delta
			);

			return true;
		case IS_ARRAY:
			arr = (zend_array *) user_cache_shared_graph_rebase_pointer(
				Z_ARR_P(value),
				ctx->old_base,
				ctx->len,
				ctx->delta
			);

			Z_ARR_P(value) = arr;

			if (!user_cache_shared_graph_pointer_in_range(arr, ctx->new_base, ctx->len)) {
				return true;
			}

			return user_cache_shared_graph_rebase_verbatim_array(ctx, arr);
		default:
			return true;
	}
}

static bool user_cache_shared_graph_rebase_verbatim_array(
		php_user_cache_shared_graph_rebase_context *ctx,
		zend_array *arr)
{
	zval *packed;
	Bucket *bucket;
	uint32_t i;
	void *data;

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	if (!user_cache_shared_graph_pointer_in_range(
			arr,
			ctx->new_base,
			ctx->len
		)
	) {
		return true;
	}

	if (!php_user_cache_seen_test_and_add(ctx->seen, arr)) {
		return true;
	}

	data = HT_GET_DATA_ADDR(arr);
	data = user_cache_shared_graph_rebase_pointer(data, ctx->old_base, ctx->len, ctx->delta);

	HT_SET_DATA_ADDR(arr, data);

	if (!user_cache_shared_graph_pointer_in_range(data, ctx->new_base, ctx->len)) {
		return false;
	}

	if (HT_IS_PACKED(arr)) {
		packed = arr->arPacked;
		for (i = 0; i < arr->nNumUsed; i++) {
			if (!user_cache_shared_graph_rebase_verbatim_zval(ctx, &packed[i])) {
				return false;
			}
		}
	} else {
		bucket = arr->arData;
		for (i = 0; i < arr->nNumUsed; i++) {
			if (bucket[i].key != NULL) {
				bucket[i].key = (zend_string *) user_cache_shared_graph_rebase_pointer(
					bucket[i].key,
					ctx->old_base,
					ctx->len,
					ctx->delta
				);

				if (!user_cache_shared_graph_pointer_in_range(bucket[i].key, ctx->new_base, ctx->len)) {
					return false;
				}
			}

			if (!user_cache_shared_graph_rebase_verbatim_zval(ctx, &bucket[i].val)) {
				return false;
			}
		}
	}

	return true;
}

static bool user_cache_shared_graph_rebase_graph_value(
		php_user_cache_shared_graph_rebase_context *ctx,
		const php_user_cache_shared_graph_value *value)
{
	const php_user_cache_shared_graph_array *graph_array;
	const php_user_cache_shared_graph_array_element *graph_elems;
	const php_user_cache_shared_graph_shaped_array *graph_shaped_array;
	const php_user_cache_shared_graph_value *graph_vals;
	const php_user_cache_shared_graph_object *graph_obj;
	const php_user_cache_shared_graph_property *props;
	const php_user_cache_shared_graph_safe_direct_object *graph_safe_direct;
	const php_user_cache_shared_graph_shaped_state_object *graph_shaped_state;
	const php_user_cache_shared_graph_state_schema *state_schema;
	zend_array *arr;
	uint32_t i;

	if (php_user_cache_stack_overflowed()) {
		return false;
	}

	switch (value->type) {
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
			if ((uint32_t) value->payload.offset == 0) {
				return true;
			}

			arr = (zend_array *) (void *) (ctx->new_base + (uint32_t) value->payload.offset);

			return user_cache_shared_graph_rebase_verbatim_array(ctx, arr);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
			graph_array = (const php_user_cache_shared_graph_array *) (ctx->new_base + (uint32_t) value->payload.offset);
			graph_elems = (const php_user_cache_shared_graph_array_element *) (ctx->new_base + graph_array->elements_offset);

			for (i = 0; i < graph_array->count; i++) {
				if (!user_cache_shared_graph_rebase_graph_value(ctx, &graph_elems[i].value)) {
					return false;
				}
			}

			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
			graph_shaped_array = (const php_user_cache_shared_graph_shaped_array *) (ctx->new_base + (uint32_t) value->payload.offset);
			graph_vals = (const php_user_cache_shared_graph_value *) (ctx->new_base + graph_shaped_array->values_offset);

			for (i = 0; i < graph_shaped_array->count; i++) {
				if (!user_cache_shared_graph_rebase_graph_value(ctx, &graph_vals[i])) {
					return false;
				}
			}

			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
			graph_obj = (const php_user_cache_shared_graph_object *) (ctx->new_base + (uint32_t) value->payload.offset);
			props = (const php_user_cache_shared_graph_property *) (ctx->new_base + graph_obj->properties_offset);
			for (i = 0; i < graph_obj->property_count; i++) {
				if (!user_cache_shared_graph_rebase_graph_value(ctx, &props[i].value)) {
					return false;
				}
			}

			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_REFERENCE:
			return user_cache_shared_graph_rebase_graph_value(
				ctx,
				&((const php_user_cache_shared_graph_reference *) (ctx->new_base + (uint32_t) value->payload.offset))->inner
			);
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
			graph_shaped_state =
				(const php_user_cache_shared_graph_shaped_state_object *) (ctx->new_base + (uint32_t) value->payload.offset)
			;
			state_schema =
				(const php_user_cache_shared_graph_state_schema *) (ctx->new_base + graph_shaped_state->state_schema_offset)
			;
			graph_vals = (const php_user_cache_shared_graph_value *) (ctx->new_base + graph_shaped_state->state_values_offset);

			for (i = 0; i < state_schema->count; i++) {
				if (!user_cache_shared_graph_rebase_graph_value(ctx, &graph_vals[i])) {
					return false;
				}
			}

			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
			ZEND_FALLTHROUGH;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT: {
			graph_safe_direct =
				(const php_user_cache_shared_graph_safe_direct_object *) (ctx->new_base + (uint32_t) value->payload.offset)
			;

			if (!user_cache_shared_graph_rebase_graph_value(ctx, &graph_safe_direct->state)) {
				return false;
			}

			props = (const php_user_cache_shared_graph_property *) (ctx->new_base + graph_safe_direct->properties_offset);
			for (i = 0; i < graph_safe_direct->property_count; i++) {
				if (!user_cache_shared_graph_rebase_graph_value(ctx, &props[i].value)) {
					return false;
				}
			}

			return true;
		}
		default:
			return true;
	}
}
#endif

static void user_cache_shared_graph_force_retire_locked(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header = user_cache_shared_graph_payload_header(payload_offset);
	int state, expected;

	if (header == NULL) {
		return;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);

		if ((state & PHP_USER_CACHE_SHARED_GRAPH_RETIRED) != 0) {
			return;
		}

		expected = state;
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state | PHP_USER_CACHE_SHARED_GRAPH_RETIRED)) {
			return;
		}
	}
}

static int user_cache_shared_graph_orphan_offset_compare(const void *a, const void *b)
{
	uint32_t lhs = *(const uint32_t *) a;
	uint32_t rhs = *(const uint32_t *) b;

	return lhs < rhs ? -1 : (lhs > rhs ? 1 : 0);
}

/* Snapshot live offsets once for orphan recovery. */
static uint32_t user_cache_shared_graph_snapshot_live_offsets_locked(
	php_user_cache_header *header,
	uint32_t **offsets_out
)
{
	php_user_cache_entry *entries, *entry;
	uint32_t *offsets;
	uint32_t i, count;

	entries = php_user_cache_entries_ptr(header);
	offsets = emalloc((size_t) header->capacity * 2 * sizeof(*offsets));
	count = 0;

	for (i = 0; i < header->capacity; i++) {
		entry = &entries[i];
		if (entry->state != PHP_USER_CACHE_ENTRY_USED) {
			continue;
		}

		if (entry->key_offset != 0) {
			offsets[count++] = entry->key_offset;
		}

		if (entry->value_offset != 0) {
			offsets[count++] = entry->value_offset;
		}
	}

	qsort(offsets, count, sizeof(*offsets), user_cache_shared_graph_orphan_offset_compare);

	*offsets_out = offsets;

	return count;
}

static bool user_cache_orphaned_graph_block_is_referenced_locked(
	const uint32_t *live_offsets,
	uint32_t live_offset_count,
	uint32_t block_offset,
	uint32_t block_size
)
{
	uint32_t payload_start, lo, hi, mid;

	payload_start = block_offset + (uint32_t) sizeof(php_user_cache_block);
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

static bool user_cache_shared_graph_reclaim_orphaned_by_scan_locked(php_user_cache_header *header)
{
	php_user_cache_block *block;
	php_user_cache_shared_graph_header *graph_header;
	uint32_t *live_offsets;
	uint32_t used_end, offset, block_size, payload_offset, live_offset_count;
	bool reclaimed = false, restart;

	live_offset_count = user_cache_shared_graph_snapshot_live_offsets_locked(header, &live_offsets);

	do {
		restart = false;
		used_end = header->data_offset + header->next_free;
		offset = header->data_offset;

		while (offset < used_end) {
			block = php_user_cache_block_ptr(offset);
			block_size = block->size;

			if (block_size < PHP_USER_CACHE_ALIGNED_SIZE(sizeof(php_user_cache_block) + 1) ||
				block_size > used_end - offset
			) {
				goto done;
			}

			if (!php_user_cache_block_is_free(block) &&
				!user_cache_orphaned_graph_block_is_referenced_locked(
					live_offsets, live_offset_count, offset, block_size
				)
			) {
				payload_offset = offset + (uint32_t) sizeof(php_user_cache_block);
				graph_header = user_cache_shared_graph_payload_header(payload_offset);

				if (graph_header != NULL &&
					zend_atomic_int_load_ex(&graph_header->ref_state) == PHP_USER_CACHE_SHARED_GRAPH_RETIRED
				) {
					php_user_cache_free_locked(payload_offset);
					reclaimed = true;
					restart = true;

					break;
				}
			}

			offset += block_size;
		}
	} while (restart);

done:
	efree(live_offsets);

	return reclaimed;
}

static bool user_cache_shared_graph_load_root_value(
	const php_user_cache_shared_graph_header *header,
	size_t buf_len,
	php_user_cache_shared_graph_value *root_value
)
{
	uint32_t root_type;

	if (header->root_offset != 0 && header->root_offset >= buf_len) {
		return false;
	}

	root_type = header->root_type != 0 ? header->root_type : PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT;

	memset(root_value, 0, sizeof(*root_value));

	root_value->type = (uint8_t) root_type;
	root_value->payload.offset = header->root_offset;

	switch (root_type) {
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING:
			return true;
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT:
		case PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT:
			return header->root_offset != 0;
		default:
			return false;
	}
}

#if ZEND_DEBUG
static bool user_cache_shared_graph_rebase_payload_pointers(
	const uint8_t *buf,
	size_t graph_len,
	const uint8_t *old_base,
	ptrdiff_t delta)
{
	const php_user_cache_shared_graph_header *header;
	php_user_cache_shared_graph_rebase_context ctx;
	php_user_cache_shared_graph_value root_value;
	HashTable seen_arrs;
	bool result;

	header = (const php_user_cache_shared_graph_header *) buf;

	/* This path has not called shared_graph_locate(). */
	if (!user_cache_shared_graph_header_is_valid(header) ||
		!user_cache_shared_graph_load_root_value(header, graph_len, &root_value)
	) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	ctx.old_base = old_base;
	ctx.new_base = buf;
	ctx.len = graph_len;
	ctx.delta = delta;
	ctx.seen = &seen_arrs;

	result = user_cache_shared_graph_rebase_graph_value(&ctx, &root_value);

	zend_hash_destroy(&seen_arrs);

	return result;
}

/* Verify that relocation left no pointers to rewrite. */
static void user_cache_shared_graph_check_rebase_complete(
	const uint8_t *dst_base,
	size_t graph_len,
	const uint8_t *src_base)
{
	uint8_t *check_buf;
	bool result;

	check_buf = emalloc(graph_len);
	memcpy(check_buf, dst_base, graph_len);

	result = user_cache_shared_graph_rebase_payload_pointers(
		dst_base,
		graph_len,
		src_base,
		(ptrdiff_t) (src_base - dst_base)
	);
	ZEND_ASSERT(result);
	ZEND_ASSERT(memcmp(check_buf, dst_base, graph_len) == 0);

	efree(check_buf);
}
#else
# define user_cache_shared_graph_check_rebase_complete(dst_base, graph_len, src_base)
#endif

static void user_cache_destroy_shared_graph_ref_index(void)
{
	if (UC_G(shared_graph_ref_index) != NULL) {
		zend_hash_destroy(UC_G(shared_graph_ref_index));
		efree(UC_G(shared_graph_ref_index));

		UC_G(shared_graph_ref_index) = NULL;
	}
}

static void user_cache_shared_graph_refs_check_fork(void)
{
	uint64_t pid = php_user_cache_cached_pid();

	if (UC_G(shared_graph_ref_owner_pid) == pid) {
		return;
	}

	if (UC_G(shared_graph_ref_owner_pid) != 0) {
		if (UC_G(shared_graph_refs) != NULL) {
			efree(UC_G(shared_graph_refs));

			UC_G(shared_graph_refs) = NULL;
		}

		UC_G(shared_graph_ref_count) = 0;
		UC_G(shared_graph_ref_capacity) = 0;

		user_cache_destroy_shared_graph_ref_index();
	}

	UC_G(shared_graph_ref_owner_pid) = pid;
}

static void user_cache_grow_shared_graph_refs(
		php_user_cache_shared_graph_ref **refs,
		uint32_t *capacity,
		uint32_t initial_capacity)
{
	*capacity = *capacity == 0 ? initial_capacity : *capacity * 2;
	*refs = erealloc(*refs, sizeof(**refs) * *capacity);
}

static void user_cache_ensure_shared_graph_ref_index(void)
{
	if (UC_G(shared_graph_ref_index) == NULL) {
		UC_G(shared_graph_ref_index) = emalloc(sizeof(HashTable));
		zend_hash_init(UC_G(shared_graph_ref_index), 8, NULL, NULL, 0);
	}
}

/* Include the context to disambiguate equal offsets in different pools. */
static zend_ulong user_cache_shared_graph_ref_index_key(
	const php_user_cache_context *ctx,
	uint32_t payload_offset
)
{
	return ((zend_ulong) (uintptr_t) ctx) ^ (zend_ulong) payload_offset;
}

PHP_USER_CACHE_DECODE_HOT bool php_user_cache_shared_graph_update_object_property(
	zval *obj_zv,
	zend_string *prop_name,
	zval *prop_val
)
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
	if (user_cache_shared_graph_is_unmangled_property_name(prop_name)) {
		prop_info = zend_get_property_info(obj->ce, prop_name, true);
		if (user_cache_shared_graph_try_update_declared_property(
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

			if (props != NULL) {
				Z_TRY_ADDREF_P(prop_val);

				zend_hash_update(props, prop_name, prop_val);

				return true;
			}
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

PHP_USER_CACHE_DECODE_HOT bool php_user_cache_shared_graph_update_object_property_at(
		zval *obj_zv,
		zend_string *prop_name,
		uint32_t prop_idx,
		zval *prop_val
)
{
	zend_object *obj;
	zend_property_info *prop_info;
	bool failed;

	obj = Z_OBJ_P(obj_zv);
	if (user_cache_shared_graph_is_unmangled_property_name(prop_name) &&
		obj->ce->type == ZEND_USER_CLASS &&
		obj->ce->properties_info_table != NULL &&
		prop_idx < obj->ce->default_properties_count
	) {
		prop_info = obj->ce->properties_info_table[prop_idx];

		if (user_cache_shared_graph_try_update_declared_property(
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

	return php_user_cache_shared_graph_update_object_property(
		obj_zv,
		prop_name,
		prop_val
	);
}

void php_user_cache_decode_resolve_cache_release(void)
{
	user_cache_shared_graph_object_route_memo_release();

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

void php_user_cache_decode_shape_prototype_cache_release(void)
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
void php_user_cache_decode_maps_teardown(void)
{
	user_cache_decode_identity_map_teardown();
	user_cache_decode_reference_map_teardown();
	user_cache_decode_array_map_teardown();
}

bool php_user_cache_shared_graph_prefers_prototype(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header =
		user_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & PHP_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE) != 0;
}

bool php_user_cache_shared_graph_payload_has_aliases(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header =
		user_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return true;
	}

	return (header->flags & PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY) != 0;
}

bool php_user_cache_shared_graph_decode_is_lock_safe(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header =
		user_cache_shared_graph_payload_header(payload_offset)
	;

	return header != NULL &&
		(header->flags & PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT) == 0
	;
}

/* Prove verbatim eligibility and compute the root size in one walk. */
php_user_cache_verbatim_root_result php_user_cache_shared_graph_calc_verbatim_root(
	const zval *value,
	HashTable *verbatim_verdicts,
	size_t *buf_len)
{
	php_user_cache_shared_graph_calc_context calc_ctx;
	php_user_cache_verbatim_root_result result;
	zval verdict_zv;
	bool immutable, completed;

	if (!user_cache_shared_graph_can_use_verbatim_arrays() ||
		Z_ARRVAL_P(value)->nNumOfElements == 0
	) {
		return PHP_USER_CACHE_VERBATIM_ROOT_UNDECIDED;
	}

	immutable = (GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) != 0;

	user_cache_shared_graph_calc_init(&calc_ctx);
	calc_ctx.verbatim_arrays_allowed = true;
	calc_ctx.state_memo = NULL;

	completed = user_cache_shared_graph_calc_reserve(
			&calc_ctx,
			sizeof(php_user_cache_shared_graph_header)
		) &&
		user_cache_shared_graph_calc_verbatim_value(&calc_ctx, value)
	;

	if (completed && calc_ctx.size <= UINT32_MAX - (ZEND_MM_ALIGNMENT - 1)) {
		*buf_len = calc_ctx.size + (ZEND_MM_ALIGNMENT - 1);
		result = PHP_USER_CACHE_VERBATIM_ROOT_SIZED;
	} else if (immutable || completed) {
		/* Immutable arrays cannot fail verbatim eligibility. */
		result = PHP_USER_CACHE_VERBATIM_ROOT_ELIGIBLE_UNSIZED;
	} else if (calc_ctx.reserve_failed) {
		result = PHP_USER_CACHE_VERBATIM_ROOT_UNDECIDED;
	} else {
		result = PHP_USER_CACHE_VERBATIM_ROOT_INELIGIBLE;
	}

	user_cache_shared_graph_calc_destroy(&calc_ctx);

	if (!immutable && result != PHP_USER_CACHE_VERBATIM_ROOT_UNDECIDED) {
		ZVAL_BOOL(&verdict_zv, result != PHP_USER_CACHE_VERBATIM_ROOT_INELIGIBLE);
		zend_hash_index_add(verbatim_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value), &verdict_zv);
	}

	return result;
}

bool php_user_cache_shared_graph_can_copy_verbatim_root(const zval *value, HashTable *verbatim_verdicts)
{
	HashTable seen_arrs, direct_verdicts;
	zval verdict_zv;
	bool eligible;

	if (!user_cache_shared_graph_can_use_verbatim_arrays()) {
		return false;
	}

	if (GC_FLAGS(Z_ARRVAL_P(value)) & IS_ARRAY_IMMUTABLE) {
		return true;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);
	zend_hash_init(&direct_verdicts, 8, NULL, NULL, 0);

	eligible = user_cache_shared_graph_can_copy_verbatim_value(
		&seen_arrs,
		&direct_verdicts,
		value
	);

	zend_hash_destroy(&direct_verdicts);
	zend_hash_destroy(&seen_arrs);

	ZVAL_BOOL(&verdict_zv, eligible);
	zend_hash_index_add(verbatim_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(value), &verdict_zv);

	return eligible;
}

bool php_user_cache_calculate_shared_graph_size(
	const zval *value,
	HashTable *state_memo,
	HashTable *verbatim_verdicts,
	size_t *buf_len
)
{
	php_user_cache_shared_graph_calc_context calc_ctx;
	zend_class_entry *root_ce;
	bool result;

	if (!buf_len) {
		return false;
	}

	*buf_len = 0;

	if (Z_TYPE_P(value) == IS_OBJECT) {
		root_ce = Z_OBJCE_P(value);
		if (!(root_ce->ce_flags & ZEND_ACC_ENUM) &&
			user_cache_shared_graph_classify_object_route(root_ce) == PHP_USER_CACHE_OBJECT_ROUTE_UNSTORABLE
		) {
			return false;
		}
	} else if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_STRING) {
		return false;
	}

	user_cache_shared_graph_calc_init(&calc_ctx);

	calc_ctx.verbatim_arrays_allowed = user_cache_shared_graph_can_use_verbatim_arrays();
	calc_ctx.shared_verdicts = verbatim_verdicts;
	calc_ctx.state_memo = state_memo;
	result = user_cache_shared_graph_calc_reserve(
		&calc_ctx,
		sizeof(php_user_cache_shared_graph_header)
	);

	if (result) {
		result = user_cache_shared_graph_calc_value(&calc_ctx, value);
	}

	if (result) {
		if (calc_ctx.size > UINT32_MAX - (ZEND_MM_ALIGNMENT - 1)) {
			result = false;
		} else {
			calc_ctx.size += ZEND_MM_ALIGNMENT - 1;
		}
	}

	if (result) {
		*buf_len = calc_ctx.size;
	}

	user_cache_shared_graph_calc_destroy(&calc_ctx);

	return result;
}

bool php_user_cache_build_shared_graph_in_place(
	const zval *value,
	HashTable *state_memo,
	const HashTable *verbatim_verdicts,
	uint8_t *buf,
	size_t buf_len,
	size_t *graph_len,
	bool *has_verbatim_array,
	uint32_t **fixup_offsets,
	uint32_t *fixup_count
)
{
	php_user_cache_shared_graph_copy_context copy_ctx;
	php_user_cache_shared_graph_header *header;
	php_user_cache_shared_graph_value root_value;
	uint32_t header_offset, root_offset, root_type;
	size_t padding;
	bool result;

	if (buf == NULL) {
		return false;
	}

	padding = user_cache_shared_graph_alignment_padding(buf);
	if (padding > buf_len || buf_len - padding < sizeof(php_user_cache_shared_graph_header)) {
		return false;
	}

	if (padding != 0) {
		memset(buf, 0, padding);
	}

	buf += padding;
	buf_len -= padding;

	user_cache_shared_graph_copy_init(&copy_ctx, buf, buf_len);

	copy_ctx.shared_verdicts = verbatim_verdicts;
	copy_ctx.state_memo = state_memo;
	root_offset = 0;
	root_type = 0;

	result = user_cache_shared_graph_copy_alloc(&copy_ctx, sizeof(*header), &header_offset) && header_offset == 0;
	if (result) {
		if (Z_TYPE_P(value) == IS_OBJECT) {
			result = user_cache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				(
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SAFE_DIRECT_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERIALIZED_SHAPED_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SERDES_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SLEEP_SHAPED_OBJECT ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_ENUM
				)
			;
		} else if (Z_TYPE_P(value) == IS_ARRAY) {
			result = user_cache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				(
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_ARRAY ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_DYNAMIC_ARRAY ||
					root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_SHAPED_ARRAY
				)
			;
		} else if (Z_TYPE_P(value) == IS_STRING) {
			result = user_cache_shared_graph_copy_value(&copy_ctx, value, &root_value) &&
				root_value.type == PHP_USER_CACHE_SHARED_GRAPH_VALUE_STRING
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
		user_cache_shared_graph_copy_destroy(&copy_ctx);

		return false;
	}

	header = (php_user_cache_shared_graph_header *) buf;
	header->magic = PHP_USER_CACHE_SHARED_GRAPH_MAGIC;
	header->version = PHP_USER_CACHE_SHARED_GRAPH_VERSION;
	header->root_offset = root_offset;
	header->root_type = root_type;
	header->flags =
		(copy_ctx.has_shared_identity ? PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_SHARED_IDENTITY : 0) |
		(copy_ctx.has_object ? PHP_USER_CACHE_SHARED_GRAPH_FLAG_HAS_OBJECT : 0) |
		((copy_ctx.prefers_prototype && !copy_ctx.has_userland_restore_object) ? PHP_USER_CACHE_SHARED_GRAPH_FLAG_PREFERS_PROTOTYPE : 0)
	;

	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);

	if (graph_len != NULL) {
		*graph_len = copy_ctx.position;
	}

	if (has_verbatim_array != NULL) {
		*has_verbatim_array = copy_ctx.has_verbatim_array;
	}

	ZEND_ASSERT(copy_ctx.has_verbatim_array || copy_ctx.fixup_count == 0);

	/* Transfer relocation fixups to the prepared value. */
	if (fixup_offsets != NULL) {
		*fixup_offsets = copy_ctx.fixup_offsets;
		*fixup_count = copy_ctx.fixup_count;

		copy_ctx.fixup_offsets = NULL;
		copy_ctx.fixup_count = 0;
		copy_ctx.fixup_capacity = 0;
	}

	user_cache_shared_graph_copy_destroy(&copy_ctx);

	return true;
}

bool php_user_cache_shared_graph_copy_fits_buffer(
	const uint8_t *dst_buf,
	size_t dst_buf_len,
	const uint8_t *src_buf,
	size_t src_buf_len,
	size_t src_graph_len
)
{
	size_t dst_padding;

	if (src_buf == NULL || dst_buf == NULL || src_graph_len == 0 || src_graph_len > src_buf_len) {
		return false;
	}

	dst_padding = user_cache_shared_graph_alignment_padding(dst_buf);

	return dst_padding <= dst_buf_len &&
		(src_graph_len <= dst_buf_len - dst_padding)
	;
}

bool php_user_cache_shared_graph_decode(
	const uint8_t *buf,
	size_t buf_len,
	zval *dst
)
{
	const php_user_cache_shared_graph_header *header;
	const uint8_t *graph_buf;
	php_user_cache_shared_graph_value root_value;
	HashTable *saved_identity_map, *saved_reference_map, *saved_array_map;
	bool result;

	graph_buf = user_cache_shared_graph_locate(
		buf,
		buf_len,
		&buf_len
	);
	if (graph_buf == NULL) {
		return false;
	}

	buf = graph_buf;

	header = (const php_user_cache_shared_graph_header *) buf;
	if (!user_cache_shared_graph_load_root_value(header, buf_len, &root_value)) {
		return false;
	}

	/* Restore hooks may re-enter fetch. */
	saved_identity_map = UC_G(decode_identity_map);
	UC_G(decode_identity_map) = NULL;
	saved_reference_map = UC_G(decode_reference_map);
	UC_G(decode_reference_map) = NULL;
	saved_array_map = UC_G(decode_array_map);
	UC_G(decode_array_map) = NULL;

	result = user_cache_shared_graph_decode_value(buf, buf_len, &root_value, dst);

	user_cache_decode_identity_map_teardown();
	user_cache_decode_reference_map_teardown();
	user_cache_decode_array_map_teardown();

	UC_G(decode_identity_map) = saved_identity_map;
	UC_G(decode_reference_map) = saved_reference_map;
	UC_G(decode_array_map) = saved_array_map;

	/* Drop address-keyed caches after releasing a failed payload. */
	if (!result) {
		php_user_cache_decode_resolve_cache_release();
		php_user_cache_decode_shape_prototype_cache_release();
	}

	return result;
}

bool php_user_cache_shared_graph_can_overwrite_payload_locked(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header =
		user_cache_shared_graph_payload_header(payload_offset)
	;

	if (header == NULL) {
		return false;
	}

	if (!php_user_cache_quiesce_graph_payloads_locked()) {
		return false;
	}

	return zend_atomic_int_load_ex(&header->ref_state) == 0;
}

bool php_user_cache_shared_graph_publish_copied_payload_locked(
	uint8_t *dst_buf,
	size_t dst_buf_len,
	const uint8_t *src_buf,
	size_t src_buf_len,
	size_t src_graph_len,
	bool has_verbatim_array,
	const uint32_t *fixup_offsets,
	uint32_t fixup_count)
{
	const uint8_t *src_base;
	php_user_cache_shared_graph_header *header;
	uint8_t *dst_base;
	uintptr_t delta;
	size_t src_padding, dst_padding;
	uint32_t i;

	if (!php_user_cache_shared_graph_copy_fits_buffer(
			dst_buf,
			dst_buf_len,
			src_buf,
			src_buf_len,
			src_graph_len
		)
	) {
		return false;
	}

	src_padding = user_cache_shared_graph_alignment_padding(src_buf);
	if (src_padding > src_buf_len ||
		src_buf_len - src_padding < src_graph_len ||
		src_graph_len < sizeof(*header)
	) {
		return false;
	}

	src_base = src_buf + src_padding;
	dst_padding = user_cache_shared_graph_alignment_padding(dst_buf);
	if (dst_padding != 0) {
		memset(dst_buf, 0, dst_padding);
	}

	dst_base = dst_buf + dst_padding;

	if (src_base != dst_base) {
		memcpy(dst_base, src_base, src_graph_len);
	}

	header = (php_user_cache_shared_graph_header *) dst_base;
	ZEND_ATOMIC_INT_INIT(&header->ref_state, 0);

	if (src_base == dst_base) {
		return true;
	}

	/* Only non-empty verbatim arrays contain absolute pointers. */
	if (!has_verbatim_array) {
		ZEND_ASSERT(fixup_count == 0);

		user_cache_shared_graph_check_rebase_complete(dst_base, src_graph_len, src_base);

		return true;
	}

	/* Patch every recorded absolute pointer slot. */
	ZEND_ASSERT(fixup_offsets != NULL && fixup_count > 0);

	delta = (uintptr_t) dst_base - (uintptr_t) src_base;
	for (i = 0; i < fixup_count; i++) {
		ZEND_ASSERT(fixup_offsets[i] <= src_graph_len - sizeof(uintptr_t));
		ZEND_ASSERT((((uintptr_t) dst_base + fixup_offsets[i]) & (sizeof(uintptr_t) - 1)) == 0);

		*(uintptr_t *) (void *) (dst_base + fixup_offsets[i]) += delta;
	}

	user_cache_shared_graph_check_rebase_complete(dst_base, src_graph_len, src_base);

	return true;
}

void php_user_cache_shared_graph_orphan_payload_locked(uint32_t payload_offset)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	uint32_t i;

	if (header == NULL || payload_offset == 0) {
		return;
	}

	user_cache_shared_graph_force_retire_locked(payload_offset);

	for (i = 0; i < PHP_USER_CACHE_ORPHANED_GRAPH_SLOTS; i++) {
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

void php_user_cache_shared_graph_reclaim_orphaned_locked(void)
{
	php_user_cache_header *header = php_user_cache_header_ptr();
	php_user_cache_shared_graph_header *graph_header;
	uint32_t i, payload_offset;
	bool checked_quiescent = false;

	if (header == NULL) {
		return;
	}

	for (i = 0; i < PHP_USER_CACHE_ORPHANED_GRAPH_SLOTS; i++) {
		payload_offset = header->orphaned_graphs[i];
		if (payload_offset == 0) {
			continue;
		}

		if (!checked_quiescent) {
			if (!php_user_cache_quiesce_graph_payloads_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		graph_header = user_cache_shared_graph_payload_header(payload_offset);

		if (graph_header == NULL) {
			header->orphaned_graphs[i] = 0;

			continue;
		}

		if (zend_atomic_int_load_ex(&graph_header->ref_state) !=
			PHP_USER_CACHE_SHARED_GRAPH_RETIRED
		) {
			header->orphaned_graphs[i] = 0;

			continue;
		}

		header->orphaned_graphs[i] = 0;

		php_user_cache_free_locked(payload_offset);
	}

	if (header->orphaned_graphs_saturated != 0) {
		if (!checked_quiescent) {
			if (!php_user_cache_quiesce_graph_payloads_locked()) {
				return;
			}

			checked_quiescent = true;
		}

		user_cache_shared_graph_reclaim_orphaned_by_scan_locked(header);

		header->orphaned_graphs_saturated = 0;
	}
}

bool php_user_cache_shared_graph_acquire_ref(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header = user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if ((state & PHP_USER_CACHE_SHARED_GRAPH_RETIRED) != 0 ||
			refcount == PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK
		) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, state + 1)) {
			return true;
		}
	}
}

/* Abnormally terminated owners can leave retired payloads pinned. Reclaiming
 * them safely requires per-owner reference records and a layout version bump. */
bool php_user_cache_shared_graph_retire_payload_locked(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header = user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected;

	if (header == NULL) {
		return true;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return true;
		}

		if ((state & PHP_USER_CACHE_SHARED_GRAPH_RETIRED) != 0) {
			return false;
		}

		if (zend_atomic_int_compare_exchange_ex(
				&header->ref_state,
				&expected,
				(state | PHP_USER_CACHE_SHARED_GRAPH_RETIRED)
			)
		) {
			return false;
		}
	}
}

bool php_user_cache_shared_graph_release_ref_locked(uint32_t payload_offset)
{
	php_user_cache_shared_graph_header *header = user_cache_shared_graph_payload_header(payload_offset);
	int state, refcount, expected, desired;

	if (header == NULL) {
		return false;
	}

	for (;;) {
		state = zend_atomic_int_load_ex(&header->ref_state);
		refcount = state & PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK;
		expected = state;

		if (refcount == 0) {
			return false;
		}

		desired = (state & PHP_USER_CACHE_SHARED_GRAPH_RETIRED) | (refcount - 1);
		if (zend_atomic_int_compare_exchange_ex(&header->ref_state, &expected, desired)) {
			return (desired & PHP_USER_CACHE_SHARED_GRAPH_RETIRED) != 0 &&
				(desired & PHP_USER_CACHE_SHARED_GRAPH_REFCOUNT_MASK) == 0
			;
		}
	}
}

bool php_user_cache_has_request_shared_graph_ref(uint32_t payload_offset)
{
	php_user_cache_context *ctx;
	zval *index_zv;
	uint32_t i;

	user_cache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == 0) {
		return false;
	}

	ctx = php_user_cache_active_context();

	if (UC_G(shared_graph_ref_index) != NULL) {
		index_zv = zend_hash_index_find(
			UC_G(shared_graph_ref_index),
			user_cache_shared_graph_ref_index_key(ctx, payload_offset)
		);
		if (index_zv == NULL) {
			return false;
		}

		i = (uint32_t) Z_LVAL_P(index_zv);
		if (i < UC_G(shared_graph_ref_count) &&
			UC_G(shared_graph_refs)[i].context == ctx &&
			UC_G(shared_graph_refs)[i].payload_offset == payload_offset
		) {
			return true;
		}
	}

	for (i = 0; i < UC_G(shared_graph_ref_count); i++) {
		if (UC_G(shared_graph_refs)[i].context == ctx &&
			UC_G(shared_graph_refs)[i].payload_offset == payload_offset
		) {
			return true;
		}
	}

	return false;
}

void php_user_cache_shared_graph_ref_reserve(void)
{
	user_cache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == UC_G(shared_graph_ref_capacity)) {
		user_cache_grow_shared_graph_refs(&UC_G(shared_graph_refs), &UC_G(shared_graph_ref_capacity), 8);
	}

	user_cache_ensure_shared_graph_ref_index();

	zend_hash_extend(
		UC_G(shared_graph_ref_index),
		zend_hash_num_elements(UC_G(shared_graph_ref_index)) + 1,
		0
	);
}

void php_user_cache_register_shared_graph_ref(uint32_t payload_offset)
{
	php_user_cache_context *ctx;
	zval index_zv;

	user_cache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == UC_G(shared_graph_ref_capacity)) {
		user_cache_grow_shared_graph_refs(&UC_G(shared_graph_refs), &UC_G(shared_graph_ref_capacity), 8);
	}

	ctx = php_user_cache_active_context();

	UC_G(shared_graph_refs)[UC_G(shared_graph_ref_count)].context = ctx;
	UC_G(shared_graph_refs)[UC_G(shared_graph_ref_count)].payload_offset = payload_offset;

	user_cache_ensure_shared_graph_ref_index();

	ZVAL_LONG(&index_zv, (zend_long) UC_G(shared_graph_ref_count));

	zend_hash_index_add(
		UC_G(shared_graph_ref_index),
		user_cache_shared_graph_ref_index_key(ctx, payload_offset),
		&index_zv
	);

	UC_G(shared_graph_ref_count)++;
}

bool php_user_cache_release_request_shared_graph_refs(void)
{
	php_user_cache_shared_graph_ref *ref;
	php_user_cache_context *ctx, *prev_ctx;
	uint32_t i, inner;
	bool released = false;

	user_cache_shared_graph_refs_check_fork();

	if (UC_G(shared_graph_ref_count) == 0) {
		user_cache_destroy_shared_graph_ref_index();

		return false;
	}

	for (i = 0; i < UC_G(shared_graph_ref_count); i++) {
		ctx = UC_G(shared_graph_refs)[i].context;

		if (ctx == NULL) {
			continue;
		}

		prev_ctx = php_user_cache_activate_context(ctx);

		if (php_user_cache_wlock()) {
			if (php_user_cache_header_is_initialized_locked()) {
				for (inner = i; inner < UC_G(shared_graph_ref_count); inner++) {
					ref = &UC_G(shared_graph_refs)[inner];

					if (ref->context != ctx) {
						continue;
					}

					if (ref->payload_offset == 0) {
						ref->context = NULL;
						continue;
					}

					released = true;
					if (php_user_cache_shared_graph_release_ref_locked(ref->payload_offset)) {
						if (php_user_cache_quiesce_graph_payloads_locked()) {
							php_user_cache_free_locked(ref->payload_offset);
						} else {
							php_user_cache_shared_graph_orphan_payload_locked(ref->payload_offset);
						}
					}

					ref->context = NULL;
				}

				php_user_cache_shared_graph_reclaim_orphaned_locked();
			}

			php_user_cache_unlock();
		}

		php_user_cache_restore_context(prev_ctx);
	}

	efree(UC_G(shared_graph_refs));

	UC_G(shared_graph_refs) = NULL;
	UC_G(shared_graph_ref_count) = 0;
	UC_G(shared_graph_ref_capacity) = 0;

	user_cache_destroy_shared_graph_ref_index();

	return released;
}
