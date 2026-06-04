/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
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

#include "zend_static_cache_internal.h"
#include "zend_static_cache_safe_direct.h"

typedef struct _zend_opcache_static_cache_prepare_memo_entry {
	zval root_snapshot;
	unsigned char *payload_buffer;
	size_t payload_size;
	size_t payload_used_size;
	uint32_t value_len;
	uint8_t root_type;
	bool dirty;
} zend_opcache_static_cache_prepare_memo_entry;

typedef struct _zend_opcache_static_cache_prepare_memo_dependency {
	zend_opcache_static_cache_prepare_memo_entry *single_entry;
	HashTable *entries;
} zend_opcache_static_cache_prepare_memo_dependency;

static ZEND_EXT_TLS HashTable *zend_opcache_static_cache_prepare_memo_entries = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_static_cache_prepare_memo_array_roots = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_static_cache_prepare_memo_object_roots = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_static_cache_prepare_memo_arrays = NULL;
static ZEND_EXT_TLS HashTable *zend_opcache_static_cache_prepare_memo_objects = NULL;
static ZEND_EXT_TLS bool zend_opcache_static_cache_has_prepare_memo_arrays = false;
static ZEND_EXT_TLS bool zend_opcache_static_cache_has_prepare_memo_objects = false;

static void zend_opcache_static_cache_capture_decoded_reachable_value_ex(
	zval *value,
	HashTable *seen_arrays,
	HashTable *seen_objects
);

static zend_always_inline HashTable *zend_opcache_static_cache_capture_ensure_table(HashTable **table_ptr)
{
	if (*table_ptr == NULL) {
		*table_ptr = emalloc(sizeof(HashTable));
		zend_hash_init(*table_ptr, 8, NULL, NULL, 0);
	}

	return *table_ptr;
}

static zend_always_inline void zend_opcache_static_cache_capture_clear(void)
{
	zend_opcache_static_cache_capture_handle = NULL;

	if (zend_opcache_static_cache_capture_arrays != NULL) {
		zend_hash_clean(zend_opcache_static_cache_capture_arrays);
	}

	if (zend_opcache_static_cache_capture_objects != NULL) {
		zend_hash_clean(zend_opcache_static_cache_capture_objects);
	}

	zend_opcache_static_cache_capture_available = false;
}

static void zend_opcache_static_cache_prepare_memo_entry_dtor(zval *zv)
{
	zend_opcache_static_cache_prepare_memo_entry *entry = Z_PTR_P(zv);

	if (entry == NULL) {
		return;
	}

	if (entry->payload_buffer != NULL) {
		efree(entry->payload_buffer);
	}

	zval_ptr_dtor(&entry->root_snapshot);

	efree(entry);
}

static void zend_opcache_static_cache_prepare_memo_dependency_dtor(zval *zv)
{
	zend_opcache_static_cache_prepare_memo_dependency *dependency = Z_PTR_P(zv);

	if (dependency == NULL) {
		return;
	}

	if (dependency->entries != NULL) {
		zend_hash_destroy(dependency->entries);
		efree(dependency->entries);
	}

	efree(dependency);
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_kind_from_volatile_static_attribute(
		zend_attribute *volatile_static,
		zend_class_entry *scope,
		zend_long *ttl)
{
	zend_opcache_static_cache_volatile_static_attribute_config config;
	zend_string *error = NULL;

	if (ttl != NULL) {
		*ttl = 0;
	}

	if (volatile_static == NULL) {
		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	if (!zend_opcache_static_cache_volatile_static_attribute_config_from_attribute(volatile_static, scope, &config, &error)) {
		zend_error(E_ERROR, "%s", ZSTR_VAL(error));

		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	if (ttl != NULL) {
		*ttl = config.ttl;
	}

	return config.strategy == ZEND_OPCACHE_STATIC_CACHE_STRATEGY_TRACKING
		? ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING
		: ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC
	;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_kind_from_attributes(
		const HashTable *attributes,
		zend_class_entry *scope,
		zend_long *ttl)
{
	zend_attribute *volatile_static;
	bool stable_static;

	if (ttl != NULL) {
		*ttl = 0;
	}

	if (attributes == NULL) {
		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	stable_static = zend_get_attribute_str(attributes, ZEND_STRL(ZEND_OPCACHE_STATIC_CACHE_STABLE_ATTRIBUTE)) != NULL;
	volatile_static = zend_get_attribute_str(attributes, ZEND_STRL(ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_ATTRIBUTE));

	if (stable_static && volatile_static != NULL) {
		return ZEND_OPCACHE_STATIC_CACHE_CONFLICT;
	}

	if (stable_static) {
		return ZEND_OPCACHE_STATIC_CACHE_STABLE;
	}

	if (volatile_static != NULL) {
		return zend_opcache_static_cache_kind_from_volatile_static_attribute(volatile_static, scope, ttl);
	}

	return ZEND_OPCACHE_STATIC_CACHE_NONE;
}

static bool zend_opcache_static_cache_handle_attribute_conflict(zend_opcache_static_cache_kind kind)
{
	if (kind == ZEND_OPCACHE_STATIC_CACHE_CONFLICT) {
		zend_error(E_ERROR, "OPcache\\StableStatic and OPcache\\VolatileStatic cannot be combined on the same target");

		return false;
	}

	return true;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_class_blob_kind_and_ttl(
		const zend_class_entry *ce,
		zend_long *ttl)
{
	if (ce == NULL) {
		if (ttl != NULL) {
			*ttl = 0;
		}

		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	return zend_opcache_static_cache_kind_from_attributes(ce->attributes, (zend_class_entry *) ce, ttl);
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_class_blob_kind(const zend_class_entry *ce)
{
	return zend_opcache_static_cache_class_blob_kind_and_ttl(ce, NULL);
}

static bool zend_opcache_static_cache_class_blob_enabled(const zend_class_entry *ce)
{
	zend_opcache_static_cache_kind kind = zend_opcache_static_cache_class_blob_kind(ce);

	return zend_opcache_static_cache_handle_attribute_conflict(kind) &&
		kind != ZEND_OPCACHE_STATIC_CACHE_NONE
	;
}

static zend_opcache_static_cache_context *zend_opcache_static_cache_context_for_kind(zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC || kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING
		? zend_opcache_static_cache_active_volatile_context()
		: zend_opcache_static_cache_active_stable_context()
	;
}

static const char *zend_opcache_static_cache_key_prefix_for_kind(zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_STABLE
		? "stable_static"
		: "volatile_static"
	;
}

static bool zend_opcache_static_cache_kind_tracks_reachable_arrays(
		zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING ||
		kind == ZEND_OPCACHE_STATIC_CACHE_STABLE
	;
}

static bool zend_opcache_static_cache_kind_tracks_reachable_objects(
		zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING;
}

static bool zend_opcache_static_cache_kind_tracks_reachable_mutations(
		zend_opcache_static_cache_kind kind)
{
	return zend_opcache_static_cache_kind_tracks_reachable_arrays(kind) ||
		zend_opcache_static_cache_kind_tracks_reachable_objects(kind)
	;
}

static bool zend_opcache_static_cache_kind_publishes_immediately(
		zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_STABLE ||
		kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC
	;
}

static bool zend_opcache_static_cache_kind_defers_reachable_mutation_publish(
		zend_opcache_static_cache_kind kind)
{
	return kind == ZEND_OPCACHE_STATIC_CACHE_VOLATILE_STATIC_TRACKING;
}

static bool zend_opcache_static_cache_applies_to_static_property(zend_class_entry *ce, zend_property_info *property_info)
{
	zend_opcache_static_cache_kind kind;

	if (!(property_info->flags & ZEND_ACC_STATIC) || property_info->ce != ce) {
		return false;
	}

	if (zend_opcache_static_cache_class_blob_enabled(ce)) {
		return false;
	}

	kind = zend_opcache_static_cache_kind_from_attributes(property_info->attributes, ce, NULL);

	return zend_opcache_static_cache_handle_attribute_conflict(kind) &&
		kind != ZEND_OPCACHE_STATIC_CACHE_NONE
	;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_static_property_kind_and_ttl(
		zend_class_entry *ce,
		zend_property_info *property_info,
		zend_long *ttl)
{
	zend_opcache_static_cache_kind kind;

	if (ttl != NULL) {
		*ttl = 0;
	}

	if (!zend_opcache_static_cache_applies_to_static_property(ce, property_info)) {
		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	kind = zend_opcache_static_cache_kind_from_attributes(property_info->attributes, ce, ttl);

	return kind == ZEND_OPCACHE_STATIC_CACHE_CONFLICT
		? ZEND_OPCACHE_STATIC_CACHE_NONE
		: kind
	;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_static_property_kind(zend_class_entry *ce, zend_property_info *property_info)
{
	return zend_opcache_static_cache_static_property_kind_and_ttl(ce, property_info, NULL);
}

static bool zend_opcache_static_cache_class_blob_applies_to_property(zend_class_entry *ce, zend_property_info *property_info)
{
	return (property_info->flags & ZEND_ACC_STATIC) != 0 &&
		property_info->ce == ce &&
		zend_opcache_static_cache_class_blob_enabled(ce)
	;
}

static bool zend_opcache_static_cache_applies_to_function_static(const zend_op_array *op_array)
{
	zend_opcache_static_cache_kind kind;

	if (op_array->static_variables == NULL || op_array->function_name == NULL || op_array->scope == NULL) {
		return false;
	}

	if (op_array->scope != NULL && zend_opcache_static_cache_class_blob_enabled(op_array->scope)) {
		return false;
	}

	kind = zend_opcache_static_cache_kind_from_attributes(op_array->attributes, op_array->scope, NULL);

	return zend_opcache_static_cache_handle_attribute_conflict(kind) &&
		kind != ZEND_OPCACHE_STATIC_CACHE_NONE
	;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_op_array_kind_and_ttl(
		const zend_op_array *op_array,
		zend_long *ttl)
{
	zend_opcache_static_cache_kind kind;

	if (ttl != NULL) {
		*ttl = 0;
	}

	if (!zend_opcache_static_cache_applies_to_function_static(op_array)) {
		return ZEND_OPCACHE_STATIC_CACHE_NONE;
	}

	kind = zend_opcache_static_cache_kind_from_attributes(op_array->attributes, op_array->scope, ttl);

	return kind == ZEND_OPCACHE_STATIC_CACHE_CONFLICT
		? ZEND_OPCACHE_STATIC_CACHE_NONE
		: kind
	;
}

static zend_opcache_static_cache_kind zend_opcache_static_cache_op_array_kind(const zend_op_array *op_array)
{
	return zend_opcache_static_cache_op_array_kind_and_ttl(op_array, NULL);
}

static bool zend_opcache_static_cache_class_blob_applies_to_function_static(const zend_op_array *op_array)
{
	return op_array->static_variables != NULL &&
		op_array->function_name != NULL &&
		op_array->scope != NULL &&
		zend_opcache_static_cache_class_blob_enabled(op_array->scope)
	;
}

static void zend_opcache_static_cache_track_attribute_class(zend_class_entry *ce)
{
	if (!zend_opcache_static_cache_attribute_classes_initialized ||
		ce->name == NULL
	) {
		return;
	}

	zend_hash_index_update_ptr(&zend_opcache_static_cache_attribute_classes, (zend_ulong)(uintptr_t) ce, ce);
}

static zend_string *zend_opcache_static_cache_static_property_key(zend_class_entry *ce, zend_string *property_name, zend_opcache_static_cache_kind kind)
{
	if (ce->name == NULL) {
		return NULL;
	}

	return zend_strpprintf(0, "%s:%s::$%s", zend_opcache_static_cache_key_prefix_for_kind(kind), ZSTR_VAL(ce->name), ZSTR_VAL(property_name));
}

static zend_string *zend_opcache_static_cache_class_blob_key(const zend_class_entry *ce, zend_opcache_static_cache_kind kind)
{
	if (ce == NULL || ce->name == NULL) {
		return NULL;
	}

	return zend_strpprintf(0, "%s_class:%s", zend_opcache_static_cache_key_prefix_for_kind(kind), ZSTR_VAL(ce->name));
}

static zend_string *zend_opcache_static_cache_function_variable_key(const zend_op_array *op_array, zend_string *var_name, zend_opcache_static_cache_kind kind)
{
	if (op_array->function_name == NULL) {
		return NULL;
	}

	if (op_array->scope != NULL && op_array->scope->name != NULL) {
		return zend_strpprintf(
			0,
			"%s:%s::%s()::$%s",
			zend_opcache_static_cache_key_prefix_for_kind(kind),
			ZSTR_VAL(op_array->scope->name),
			ZSTR_VAL(op_array->function_name),
			ZSTR_VAL(var_name)
		);
	}

	if (op_array->filename == NULL) {
		return NULL;
	}

	return zend_strpprintf(
		0,
		"%s:%s:%u:%s()::$%s",
		zend_opcache_static_cache_key_prefix_for_kind(kind),
		ZSTR_VAL(op_array->filename),
		op_array->line_start,
		ZSTR_VAL(op_array->function_name),
		ZSTR_VAL(var_name)
	);
}

static zend_string *zend_opcache_static_cache_class_blob_method_key(const zend_op_array *op_array)
{
	if (op_array == NULL ||
		op_array->scope == NULL ||
		op_array->scope->name == NULL ||
		op_array->function_name == NULL
	) {
		return NULL;
	}

	return zend_strpprintf(
		0,
		"%s::%s()",
		ZSTR_VAL(op_array->scope->name),
		ZSTR_VAL(op_array->function_name)
	);
}

static zend_opcache_static_cache_class_blob_handle *zend_opcache_static_cache_find_class_blob_handle(zend_class_entry *ce)
{
	if (!zend_opcache_static_cache_class_blob_handles_initialized || ce == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(&zend_opcache_static_cache_class_blob_handles, (zend_ulong) (uintptr_t) ce);
}

static zend_reference *zend_opcache_static_cache_create_reference(zval *value)
{
	zend_reference *ref = emalloc(sizeof(zend_reference));

	GC_SET_REFCOUNT(ref, 1);
	GC_TYPE_INFO(ref) = GC_REFERENCE;

	ref->sources.ptr = NULL;

	ZVAL_COPY_VALUE(&ref->val, value);

	return ref;
}

static zend_reference *zend_opcache_static_cache_ensure_member_reference(zval *member)
{
	zval copy;

	if (Z_ISREF_P(member)) {
		return Z_REF_P(member);
	}

	ZVAL_COPY_VALUE(&copy, member);
	ZVAL_REF(member, zend_opcache_static_cache_create_reference(&copy));

	return Z_REF_P(member);
}

static void zend_opcache_static_cache_bind_slot_to_member(zval *slot, zval *member)
{
	zend_reference *ref;

	if (slot == NULL || member == NULL) {
		return;
	}

	ref = zend_opcache_static_cache_ensure_member_reference(member);

	ZVAL_DEINDIRECT(slot);

	if (Z_ISREF_P(slot) && Z_REF_P(slot) == ref) {
		return;
	}

	zval_ptr_dtor(slot);

	GC_ADDREF(ref);
	ZVAL_REF(slot, ref);
}

static bool zend_opcache_static_cache_tracks_reachable_mutations(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	return handle != NULL && zend_opcache_static_cache_kind_tracks_reachable_mutations(handle->kind);
}

static bool zend_opcache_static_cache_tracks_reachable_arrays(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	return handle != NULL && zend_opcache_static_cache_kind_tracks_reachable_arrays(handle->kind);
}

static bool zend_opcache_static_cache_tracks_reachable_objects(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	return handle != NULL && zend_opcache_static_cache_kind_tracks_reachable_objects(handle->kind);
}

static void zend_opcache_static_cache_reset_pending_mutation(void)
{
	memset(&zend_opcache_static_cache_pending_mutation_state, 0, sizeof(zend_opcache_static_cache_pending_mutation_state));
}

static bool zend_opcache_static_cache_has_mutable_immediate_root(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	return handle != NULL &&
		zend_opcache_static_cache_kind_publishes_immediately(handle->kind) &&
		(handle->original_root_type == IS_ARRAY || handle->original_root_type == IS_OBJECT)
	;
}

static void zend_opcache_static_cache_tracked_dependency_dtor(zval *zv)
{
	zend_opcache_static_cache_tracked_dependency *dependency = Z_PTR_P(zv);

	if (dependency == NULL) {
		return;
	}

	if (dependency->handles != NULL) {
		zend_hash_destroy(dependency->handles);
		efree(dependency->handles);
	}

	efree(dependency);
}

static void zend_opcache_static_cache_tracked_dependency_add_handle(
		zend_opcache_static_cache_tracked_dependency *dependency,
		zend_opcache_static_cache_static_slot_handle *handle)
{
	if (dependency == NULL || handle == NULL) {
		return;
	}

	if (dependency->handles != NULL) {
		zend_hash_index_update_ptr(dependency->handles, (zend_ulong) (uintptr_t) handle, handle);

		return;
	}

	if (dependency->single_handle == NULL || dependency->single_handle == handle) {
		dependency->single_handle = handle;

		return;
	}

	dependency->handles = emalloc(sizeof(HashTable));

	zend_hash_init(dependency->handles, 2, NULL, NULL, 0);
	zend_hash_index_update_ptr(dependency->handles, (zend_ulong) (uintptr_t) dependency->single_handle, dependency->single_handle);
	zend_hash_index_update_ptr(dependency->handles, (zend_ulong) (uintptr_t) handle, handle);

	dependency->single_handle = NULL;
}

static void zend_opcache_static_cache_tracked_dependency_remove_handle(
		zend_opcache_static_cache_tracked_dependency *dependency,
		zend_opcache_static_cache_static_slot_handle *handle)
{
	if (dependency == NULL || handle == NULL) {
		return;
	}

	if (dependency->handles != NULL) {
		zend_hash_index_del(dependency->handles, (zend_ulong) (uintptr_t) handle);

		return;
	}

	if (dependency->single_handle == handle) {
		dependency->single_handle = NULL;
	}
}

static void zend_opcache_static_cache_untrack_dependency_handles(
		HashTable *dependencies,
		zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_opcache_static_cache_tracked_dependency *dependency;

	if (dependencies == NULL || handle == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_PTR(dependencies, dependency) {
		zend_opcache_static_cache_tracked_dependency_remove_handle(dependency, handle);
	} ZEND_HASH_FOREACH_END();
}

static void zend_opcache_static_cache_untrack_dependencies(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	if (handle == NULL) {
		return;
	}

	zend_opcache_static_cache_untrack_dependency_handles(zend_opcache_static_cache_tracked_arrays, handle);
	zend_opcache_static_cache_untrack_dependency_handles(zend_opcache_static_cache_tracked_objects, handle);

	zend_opcache_static_cache_last_array_ht = NULL;
	zend_opcache_static_cache_last_array_dependency = NULL;
	zend_opcache_static_cache_last_object_obj = NULL;
	zend_opcache_static_cache_last_object_dependency = NULL;
}

static zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_track_dependency(
		HashTable *dependencies,
		zend_ulong key,
		zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_opcache_static_cache_tracked_dependency *dependency;

	if (dependencies == NULL || handle == NULL) {
		return NULL;
	}

	dependency = zend_hash_index_find_ptr(dependencies, key);
	if (dependency == NULL) {
		dependency = emalloc(sizeof(*dependency));
		dependency->single_handle = NULL;
		dependency->handles = NULL;

		zend_hash_index_update_ptr(dependencies, key, dependency);
	}

	zend_opcache_static_cache_tracked_dependency_add_handle(dependency, handle);

	return dependency;
}

static void zend_opcache_static_cache_prepare_memo_dependency_add_entry(
		zend_opcache_static_cache_prepare_memo_dependency *dependency,
		zend_opcache_static_cache_prepare_memo_entry *entry)
{
	if (dependency == NULL || entry == NULL) {
		return;
	}

	if (dependency->entries != NULL) {
		zend_hash_index_update_ptr(dependency->entries, (zend_ulong) (uintptr_t) entry, entry);

		return;
	}

	if (dependency->single_entry == NULL || dependency->single_entry == entry) {
		dependency->single_entry = entry;

		return;
	}

	dependency->entries = emalloc(sizeof(HashTable));

	zend_hash_init(dependency->entries, 2, NULL, NULL, 0);
	zend_hash_index_update_ptr(dependency->entries, (zend_ulong) (uintptr_t) dependency->single_entry, dependency->single_entry);
	zend_hash_index_update_ptr(dependency->entries, (zend_ulong) (uintptr_t) entry, entry);

	dependency->single_entry = NULL;
}

static zend_opcache_static_cache_prepare_memo_dependency *zend_opcache_static_cache_prepare_memo_track_dependency(
		HashTable *dependencies,
		zend_ulong key,
		zend_opcache_static_cache_prepare_memo_entry *entry)
{
	zend_opcache_static_cache_prepare_memo_dependency *dependency;

	if (dependencies == NULL || entry == NULL) {
		return NULL;
	}

	dependency = zend_hash_index_find_ptr(dependencies, key);
	if (dependency == NULL) {
		dependency = emalloc(sizeof(*dependency));
		dependency->single_entry = NULL;
		dependency->entries = NULL;

		zend_hash_index_update_ptr(dependencies, key, dependency);
	}

	zend_opcache_static_cache_prepare_memo_dependency_add_entry(dependency, entry);

	return dependency;
}

static void zend_opcache_static_cache_prepare_memo_mark_dependency_dirty(
		zend_opcache_static_cache_prepare_memo_dependency *dependency)
{
	zend_opcache_static_cache_prepare_memo_entry *entry;

	if (dependency == NULL) {
		return;
	}

	if (dependency->entries != NULL) {
		ZEND_HASH_FOREACH_PTR(dependency->entries, entry) {
			if (entry != NULL) {
				entry->dirty = true;
			}
		} ZEND_HASH_FOREACH_END();

		return;
	}

	if (dependency->single_entry != NULL) {
		dependency->single_entry->dirty = true;
	}
}

static HashTable *zend_opcache_static_cache_prepare_memo_root_table_for_value(zval *value)
{
	if (value == NULL) {
		return NULL;
	}

	ZVAL_DEREF(value);

	if (Z_TYPE_P(value) == IS_ARRAY) {
		return zend_opcache_static_cache_prepare_memo_array_roots;
	}

	if (Z_TYPE_P(value) == IS_OBJECT) {
		return zend_opcache_static_cache_prepare_memo_object_roots;
	}

	return NULL;
}

static zend_ulong zend_opcache_static_cache_prepare_memo_root_key(zval *value)
{
	ZVAL_DEREF(value);

	return Z_TYPE_P(value) == IS_ARRAY
		? (zend_ulong) (uintptr_t) Z_ARRVAL_P(value)
		: (zend_ulong) (uintptr_t) Z_OBJ_P(value)
	;
}

static zend_opcache_static_cache_prepare_memo_entry *zend_opcache_static_cache_prepare_memo_find_root_entry(zval *value)
{
	zend_opcache_static_cache_prepare_memo_entry *entry;
	zend_ulong key;
	HashTable *roots;

	roots = zend_opcache_static_cache_prepare_memo_root_table_for_value(value);
	if (roots == NULL) {
		return NULL;
	}

	key = zend_opcache_static_cache_prepare_memo_root_key(value);
	entry = zend_hash_index_find_ptr(roots, key);
	if (entry != NULL && entry->dirty) {
		zend_hash_index_del(roots, key);
		return NULL;
	}

	return entry;
}

static zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_find_tracked_array(HashTable *ht)
{
	zend_opcache_static_cache_tracked_dependency *dependency;

	if (ht == NULL || zend_opcache_static_cache_tracked_arrays == NULL) {
		return NULL;
	}

	/* Cache both hits and misses until the tracked-pointer map mutates. */
	if (ht == zend_opcache_static_cache_last_array_ht) {
		return zend_opcache_static_cache_last_array_dependency;
	}

	dependency = zend_hash_index_find_ptr(zend_opcache_static_cache_tracked_arrays, (zend_ulong) (uintptr_t) ht);

	zend_opcache_static_cache_last_array_ht = ht;
	zend_opcache_static_cache_last_array_dependency = dependency;

	return dependency;
}

static zend_opcache_static_cache_tracked_dependency *zend_opcache_static_cache_find_tracked_object(zend_object *obj)
{
	zend_opcache_static_cache_tracked_dependency *dependency;

	if (obj == NULL || zend_opcache_static_cache_tracked_objects == NULL) {
		return NULL;
	}

	/* Cache both hits and misses until the tracked-pointer map mutates. */
	if (obj == zend_opcache_static_cache_last_object_obj) {
		return zend_opcache_static_cache_last_object_dependency;
	}

	dependency = zend_hash_index_find_ptr(zend_opcache_static_cache_tracked_objects, (zend_ulong) (uintptr_t) obj);
	zend_opcache_static_cache_last_object_obj = obj;
	zend_opcache_static_cache_last_object_dependency = dependency;

	return dependency;
}

static bool zend_opcache_static_cache_slot_references(zval *slot, zend_reference *ref)
{
	if (slot == NULL || ref == NULL) {
		return false;
	}

	ZVAL_DEINDIRECT(slot);

	return Z_ISREF_P(slot) && Z_REF_P(slot) == ref;
}

static void zend_opcache_static_cache_track_reference(
		zval *slot,
		zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_reference *ref;

	if (zend_opcache_static_cache_tracked_references == NULL || slot == NULL || handle == NULL) {
		return;
	}

	ZVAL_DEINDIRECT(slot);

	if (!Z_ISREF_P(slot)) {
		return;
	}

	ref = Z_REF_P(slot);

	zend_hash_index_update_ptr(zend_opcache_static_cache_tracked_references, (zend_ulong) (uintptr_t) ref, handle);
}

static zend_opcache_static_cache_static_slot_handle *zend_opcache_static_cache_find_tracked_reference(zend_reference *ref)
{
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_ulong key;

	if (ref == NULL || zend_opcache_static_cache_tracked_roots == NULL) {
		return NULL;
	}

	key = (zend_ulong) (uintptr_t) ref;
	if (zend_opcache_static_cache_tracked_references != NULL) {
		handle = zend_hash_index_find_ptr(zend_opcache_static_cache_tracked_references, key);
		if (handle != NULL) {
			if (zend_opcache_static_cache_slot_references(handle->slot, ref)) {
				return handle;
			}

			zend_hash_index_del(zend_opcache_static_cache_tracked_references, key);
		}
	}

	ZEND_HASH_FOREACH_PTR(zend_opcache_static_cache_tracked_roots, handle) {
		if (handle != NULL && zend_opcache_static_cache_slot_references(handle->slot, ref)) {
			if (zend_opcache_static_cache_tracked_references != NULL) {
				zend_hash_index_update_ptr(zend_opcache_static_cache_tracked_references, key, handle);
			}

			return handle;
		}
	} ZEND_HASH_FOREACH_END();

	return NULL;
}

static void zend_opcache_static_cache_function_static_entry_dtor(zval *zv)
{
	zend_opcache_static_cache_function_static_entry *entry = Z_PTR_P(zv);

	if (entry == NULL) {
		return;
	}

	zend_string_release(entry->cache_key);

	if (entry->method_key != NULL) {
		zend_string_release(entry->method_key);
	}

	if (entry->var_name != NULL) {
		zend_string_release(entry->var_name);
	}

	efree(entry);
}

static void zend_opcache_static_cache_class_blob_handle_dtor(zval *zv)
{
	zend_opcache_static_cache_class_blob_handle *handle = Z_PTR_P(zv);

	if (handle == NULL) {
		return;
	}

	zend_string_release(handle->cache_key);

	if (Z_TYPE(handle->root_state) != IS_UNDEF) {
		zval_ptr_dtor(&handle->root_state);
	}

	efree(handle);
}

static zend_opcache_static_cache_class_blob_handle *zend_opcache_static_cache_ensure_class_blob_handle(zend_class_entry *ce)
{
	zend_opcache_static_cache_class_blob_handle *handle;
	zend_opcache_static_cache_kind kind;
	zend_long ttl = 0;
	zend_string *cache_key;

	if (!zend_opcache_static_cache_class_blob_handles_initialized || ce == NULL) {
		return NULL;
	}

	handle = zend_opcache_static_cache_find_class_blob_handle(ce);
	if (handle != NULL) {
		return handle;
	}

	kind = zend_opcache_static_cache_class_blob_kind_and_ttl(ce, &ttl);
	if (!zend_opcache_static_cache_handle_attribute_conflict(kind) ||
		kind == ZEND_OPCACHE_STATIC_CACHE_NONE
	) {
		return NULL;
	}

	cache_key = zend_opcache_static_cache_class_blob_key(ce, kind);
	if (cache_key == NULL) {
		return NULL;
	}

	handle = ecalloc(1, sizeof(*handle));
	handle->ce = ce;
	handle->cache_key = cache_key;
	handle->context = zend_opcache_static_cache_context_for_kind(kind);
	handle->kind = kind;
	handle->ttl = ttl;

	ZVAL_UNDEF(&handle->root_state);
	zend_hash_index_update_ptr(&zend_opcache_static_cache_class_blob_handles, (zend_ulong) (uintptr_t) ce, handle);

	return handle;
}

static void zend_opcache_static_cache_track_function_static_slot(
		zval *slot,
		zend_string *cache_key,
		zend_opcache_static_cache_kind kind,
		zend_long ttl)
{
	zend_opcache_static_cache_function_static_entry *entry;
	zend_ulong key;

	if (!zend_opcache_static_cache_function_statics_initialized || slot == NULL || cache_key == NULL) {
		return;
	}

	key = (zend_ulong) (uintptr_t) slot;
	if (zend_hash_index_exists(&zend_opcache_static_cache_function_statics, key)) {
		return;
	}

	entry = emalloc(sizeof(*entry));
	entry->slot = slot;
	entry->cache_key = zend_string_copy(cache_key);
	entry->context = zend_opcache_static_cache_context_for_kind(kind);
	entry->kind = kind;
	entry->ttl = ttl;
	entry->class_blob_handle = NULL;
	entry->method_key = NULL;
	entry->var_name = NULL;

	zend_hash_index_update_ptr(&zend_opcache_static_cache_function_statics, key, entry);
}

static void zend_opcache_static_cache_track_class_blob_function_static_slot(
		zval *slot,
		zend_opcache_static_cache_class_blob_handle *class_blob_handle,
		zend_string *method_key,
		zend_string *var_name)
{
	zend_opcache_static_cache_function_static_entry *entry;
	zend_ulong key;

	if (!zend_opcache_static_cache_function_statics_initialized ||
	    slot == NULL ||
	    class_blob_handle == NULL ||
	    method_key == NULL ||
	    var_name == NULL
	) {
		return;
	}

	key = (zend_ulong) (uintptr_t) slot;
	if (zend_hash_index_exists(&zend_opcache_static_cache_function_statics, key)) {
		return;
	}

	entry = emalloc(sizeof(*entry));
	entry->slot = slot;
	entry->cache_key = zend_string_copy(class_blob_handle->cache_key);
	entry->context = class_blob_handle->context;
	entry->kind = class_blob_handle->kind;
	entry->ttl = class_blob_handle->ttl;
	entry->class_blob_handle = class_blob_handle;
	entry->method_key = zend_string_copy(method_key);
	entry->var_name = zend_string_copy(var_name);

	zend_hash_index_update_ptr(&zend_opcache_static_cache_function_statics, key, entry);
}

static void zend_opcache_static_cache_class_blob_ensure_root_arrays(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zval root_props, root_methods;
	HashTable *root_ht;

	if (handle == NULL) {
		return;
	}

	if (Z_TYPE(handle->root_state) != IS_ARRAY) {
		if (Z_TYPE(handle->root_state) != IS_UNDEF) {
			zval_ptr_dtor(&handle->root_state);
		}

		array_init(&handle->root_state);
	}

	root_ht = Z_ARRVAL(handle->root_state);
	if (zend_hash_str_find(root_ht, ZEND_STRL("properties")) == NULL) {
		array_init(&root_props);
		zend_hash_str_update(root_ht, ZEND_STRL("properties"), &root_props);
	}

	if (zend_hash_str_find(root_ht, ZEND_STRL("methods")) == NULL) {
		array_init(&root_methods);
		zend_hash_str_update(root_ht, ZEND_STRL("methods"), &root_methods);
	}
}

static zval *zend_opcache_static_cache_class_blob_ensure_section(
		zend_opcache_static_cache_class_blob_handle *handle,
		const char *name,
		size_t name_len)
{
	zval *section, new_section;

	zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);

	section = zend_hash_str_find(Z_ARRVAL(handle->root_state), name, name_len);
	if (section != NULL && Z_TYPE_P(section) == IS_ARRAY) {
		return section;
	}

	array_init(&new_section);

	return zend_hash_str_update(Z_ARRVAL(handle->root_state), name, name_len, &new_section);
}

static HashTable *zend_opcache_static_cache_class_blob_properties_ht(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zval *properties;

	zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);
	properties = zend_hash_str_find(Z_ARRVAL(handle->root_state), ZEND_STRL("properties"));

	return (properties != NULL && Z_TYPE_P(properties) == IS_ARRAY)
		? Z_ARRVAL_P(properties)
		: NULL
	;
}

static HashTable *zend_opcache_static_cache_class_blob_methods_ht(
		zend_opcache_static_cache_class_blob_handle *handle
)
{
	zval *methods;

	zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);
	methods = zend_hash_str_find(Z_ARRVAL(handle->root_state), ZEND_STRL("methods"));

	return (methods != NULL && Z_TYPE_P(methods) == IS_ARRAY)
		? Z_ARRVAL_P(methods)
		: NULL
	;
}

static bool zend_opcache_static_cache_detach_class_blob_root_state(
		zend_opcache_static_cache_class_blob_handle *handle
)
{
	zend_string *method_key;
	zval new_root, properties_copy, methods_copy, method_copy, *properties, *methods, *method_state;

	if (handle == NULL || Z_TYPE(handle->root_state) != IS_ARRAY) {
		return false;
	}

	array_init(&new_root);

	properties = zend_hash_str_find(Z_ARRVAL(handle->root_state), ZEND_STRL("properties"));
	if (properties != NULL && Z_TYPE_P(properties) == IS_ARRAY) {
		ZVAL_ARR(&properties_copy, zend_array_dup(Z_ARRVAL_P(properties)));
	} else {
		array_init(&properties_copy);
	}

	zend_hash_str_update(Z_ARRVAL(new_root), ZEND_STRL("properties"), &properties_copy);

	methods = zend_hash_str_find(Z_ARRVAL(handle->root_state), ZEND_STRL("methods"));
	array_init_size(&methods_copy,
		(methods != NULL && Z_TYPE_P(methods) == IS_ARRAY)
			? zend_hash_num_elements(Z_ARRVAL_P(methods))
			: 0
	);

	if (methods != NULL && Z_TYPE_P(methods) == IS_ARRAY) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(methods), method_key, method_state) {
			if (method_key == NULL) {
				continue;
			}

			if (Z_TYPE_P(method_state) == IS_ARRAY) {
				ZVAL_ARR(&method_copy, zend_array_dup(Z_ARRVAL_P(method_state)));
			} else {
				ZVAL_COPY(&method_copy, method_state);
			}

			zend_hash_update(Z_ARRVAL(methods_copy), method_key, &method_copy);
		} ZEND_HASH_FOREACH_END();
	}

	zend_hash_str_update(Z_ARRVAL(new_root), ZEND_STRL("methods"), &methods_copy);

	zval_ptr_dtor(&handle->root_state);

	ZVAL_COPY_VALUE(&handle->root_state, &new_root);

	return true;
}

static bool zend_opcache_static_cache_build_default_class_blob_state(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zend_string *property_name;
	zend_property_info *property_info;
	zval copy, *source;
	HashTable *properties_ht;

	if (handle == NULL) {
		return false;
	}

	if (Z_TYPE(handle->root_state) != IS_UNDEF) {
		zval_ptr_dtor(&handle->root_state);
	}

	array_init(&handle->root_state);

	zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);

	properties_ht = zend_opcache_static_cache_class_blob_properties_ht(handle);
	if (properties_ht == NULL) {
		return false;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&handle->ce->properties_info, property_name, property_info) {
		if (property_name == NULL || !zend_opcache_static_cache_class_blob_applies_to_property(handle->ce, property_info)) {
			continue;
		}

		if (CE_STATIC_MEMBERS(handle->ce) != NULL) {
			source = CE_STATIC_MEMBERS(handle->ce) + property_info->offset;
			ZVAL_DEINDIRECT(source);
		} else {
			source = &handle->ce->default_static_members_table[property_info->offset];
		}

		ZVAL_COPY_DEREF(&copy, source);

		zend_hash_update(properties_ht, property_name, &copy);
	} ZEND_HASH_FOREACH_END();

	handle->initialized = true;
	handle->dirty = true;
	handle->request_deferred_publish = false;

	return true;
}

static bool zend_opcache_static_cache_refresh_class_blob_handle_locked(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zval cached_value;

	if (handle == NULL) {
		return false;
	}

	if (zend_opcache_static_cache_fetch_locked(handle->cache_key, &cached_value, false, NULL, false)) {
		if (Z_TYPE(cached_value) == IS_ARRAY) {
			if (Z_TYPE(handle->root_state) != IS_UNDEF) {
				zval_ptr_dtor(&handle->root_state);
			}

			ZVAL_COPY_VALUE(&handle->root_state, &cached_value);
			if (!zend_opcache_static_cache_detach_class_blob_root_state(handle)) {
				zval_ptr_dtor(&handle->root_state);

				ZVAL_UNDEF(&handle->root_state);

				if (!handle->initialized) {
					return zend_opcache_static_cache_build_default_class_blob_state(handle);
				}

				return false;
			}

			handle->initialized = true;
			handle->dirty = false;
			handle->request_deferred_publish = zend_opcache_static_cache_kind_defers_reachable_mutation_publish(handle->kind);

			zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);

			return true;
		}

		zval_ptr_dtor(&cached_value);
	}

	if (!handle->initialized) {
		return zend_opcache_static_cache_build_default_class_blob_state(handle);
	}

	zend_opcache_static_cache_class_blob_ensure_root_arrays(handle);

	return true;
}

static bool zend_opcache_static_cache_class_blob_build_snapshot(
		zend_opcache_static_cache_class_blob_handle *handle,
		zval *snapshot)
{
	zend_string *key, *var_name;
	zval properties_snapshot, methods_snapshot, method_snapshot, copy, *value, *var_value;
	HashTable *properties_ht, *methods_ht, *method_ht;

	if (handle == NULL) {
		return false;
	}

	array_init(snapshot);
	array_init(&properties_snapshot);
	array_init(&methods_snapshot);

	zend_hash_str_update(Z_ARRVAL_P(snapshot), ZEND_STRL("properties"), &properties_snapshot);
	zend_hash_str_update(Z_ARRVAL_P(snapshot), ZEND_STRL("methods"), &methods_snapshot);

	properties_ht = zend_opcache_static_cache_class_blob_properties_ht(handle);
	if (properties_ht != NULL) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(properties_ht, key, value) {
			if (key == NULL) {
				continue;
			}

			ZVAL_COPY_DEREF(&copy, value);

			zend_hash_update(Z_ARRVAL(properties_snapshot), key, &copy);
		} ZEND_HASH_FOREACH_END();
	}

	methods_ht = zend_opcache_static_cache_class_blob_methods_ht(handle);
	if (methods_ht != NULL) {
		ZEND_HASH_FOREACH_STR_KEY_VAL(methods_ht, key, value) {
			if (key == NULL || Z_TYPE_P(value) != IS_ARRAY) {
				continue;
			}

			array_init(&method_snapshot);
			method_ht = Z_ARRVAL_P(value);

			ZEND_HASH_FOREACH_STR_KEY_VAL(method_ht, var_name, var_value) {
				if (var_name == NULL) {
					continue;
				}

				ZVAL_COPY_DEREF(&copy, var_value);

				zend_hash_update(Z_ARRVAL(method_snapshot), var_name, &copy);
			} ZEND_HASH_FOREACH_END();

			zend_hash_update(Z_ARRVAL(methods_snapshot), key, &method_snapshot);
		} ZEND_HASH_FOREACH_END();
	}

	return true;
}

static bool zend_opcache_static_cache_publish_class_blob_handle_locked(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zval snapshot;
	bool result;

	if (handle == NULL) {
		return false;
	}

	ZVAL_UNDEF(&snapshot);

	if (!handle->dirty) {
		return true;
	}

	result =
		zend_opcache_static_cache_class_blob_build_snapshot(handle, &snapshot) &&
		zend_opcache_static_cache_store_locked(handle->cache_key, &snapshot, handle->ttl, false, true)
	;

	zval_ptr_dtor(&snapshot);

	if (result) {
		handle->dirty = false;
	}

	return result;
}

static void zend_opcache_static_cache_tracked_root_dtor(zval *zv)
{
	zend_opcache_static_cache_static_slot_handle *handle = Z_PTR_P(zv);

	if (handle == NULL) {
		return;
	}

	zend_string_release(handle->cache_key);
	if (handle->has_original_value) {
		zval_ptr_dtor(&handle->original_value);
	}

	efree(handle);
}

static bool zend_opcache_static_cache_value_changed(zval *current, zval *original)
{
	zval current_deref, original_deref;
	bool changed;

	ZVAL_COPY_DEREF(&current_deref, current);
	ZVAL_COPY_DEREF(&original_deref, original);

	if (Z_TYPE(current_deref) != Z_TYPE(original_deref)) {
		zval_ptr_dtor(&current_deref);
		zval_ptr_dtor(&original_deref);

		return true;
	}

	switch (Z_TYPE(current_deref)) {
		case IS_UNDEF:
		case IS_NULL:
		case IS_FALSE:
		case IS_TRUE:
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return false;
		case IS_LONG:
			changed = Z_LVAL(current_deref) != Z_LVAL(original_deref);
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return changed;
		case IS_DOUBLE:
			changed = Z_DVAL(current_deref) != Z_DVAL(original_deref);
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return changed;
		case IS_STRING:
			changed = !zend_string_equal_content(Z_STR(current_deref), Z_STR(original_deref));
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return changed;
		case IS_ARRAY:
			changed = Z_ARR(current_deref) != Z_ARR(original_deref);
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return changed;
		case IS_OBJECT:
			changed = Z_OBJ(current_deref) != Z_OBJ(original_deref);
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return changed;
		default:
			zval_ptr_dtor(&current_deref);
			zval_ptr_dtor(&original_deref);

			return true;
	}
}

static void zend_opcache_static_cache_set_tracked_root_original(zend_opcache_static_cache_static_slot_handle *handle)
{
	zval *value;

	if (handle == NULL || handle->slot == NULL) {
		return;
	}

	if (handle->has_original_value) {
		zval_ptr_dtor(&handle->original_value);
	} else {
		handle->has_original_value = true;
	}

	ZVAL_UNDEF(&handle->original_value);

	handle->original_root = NULL;
	handle->original_root_type = IS_UNDEF;
	handle->snapshot_root_published = false;

	value = handle->slot;

	ZVAL_DEINDIRECT(value);
	ZVAL_DEREF(value);

	if (zend_opcache_static_cache_kind_publishes_immediately(handle->kind) &&
		(Z_TYPE_P(value) == IS_ARRAY || Z_TYPE_P(value) == IS_OBJECT)
	) {
		/* Keep only the root identity for immediate mutable values. Taking a
		 * strong zval copy would force array COW and make nested writes look like
		 * root reassignments during request shutdown. */
		handle->original_root = Z_COUNTED_P(value);
		handle->original_root_type = Z_TYPE_P(value);

		return;
	}

	ZVAL_COPY_DEREF(&handle->original_value, value);
}

static zend_opcache_static_cache_static_slot_handle *zend_opcache_static_cache_ensure_tracked_root(
		zval *slot,
		zend_string *cache_key,
		zend_opcache_static_cache_context *context,
		zend_opcache_static_cache_kind kind,
		zend_long ttl,
		zend_opcache_static_cache_class_blob_handle *class_blob_handle)
{
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_ulong key;

	if (slot == NULL || cache_key == NULL || zend_opcache_static_cache_tracked_roots == NULL) {
		return NULL;
	}

	key = (zend_ulong) (uintptr_t) slot;
	handle = zend_hash_index_find_ptr(zend_opcache_static_cache_tracked_roots, key);
	if (handle != NULL) {
		handle->class_blob_handle = class_blob_handle;
		if (class_blob_handle != NULL) {
			handle->context = class_blob_handle->context;
			handle->kind = class_blob_handle->kind;
			handle->ttl = class_blob_handle->ttl;
			handle->request_deferred_publish = class_blob_handle->request_deferred_publish;
		} else {
			handle->context = context;
			handle->kind = kind;
			handle->ttl = ttl;
		}

		zend_opcache_static_cache_track_reference(slot, handle);

		return handle;
	}

	handle = emalloc(sizeof(*handle));
	handle->slot = slot;
	handle->cache_key = zend_string_copy(cache_key);
	handle->context = class_blob_handle != NULL ? class_blob_handle->context : context;
	handle->kind = class_blob_handle != NULL ? class_blob_handle->kind : kind;
	handle->ttl = class_blob_handle != NULL ? class_blob_handle->ttl : ttl;
	handle->class_blob_handle = class_blob_handle;

	ZVAL_UNDEF(&handle->original_value);

	handle->original_root = NULL;
	handle->original_root_type = IS_UNDEF;
	handle->has_original_value = false;
	handle->snapshot_root_published = false;
	handle->request_deferred_publish =
		class_blob_handle != NULL &&
		class_blob_handle->request_deferred_publish
	;
	handle->mutation_dirty = false;

	zend_opcache_static_cache_set_tracked_root_original(handle);
	zend_hash_index_update_ptr(zend_opcache_static_cache_tracked_roots, key, handle);
	zend_opcache_static_cache_track_reference(slot, handle);

	return handle;
}

static bool zend_opcache_static_cache_static_slot_value_changed(zend_opcache_static_cache_static_slot_handle *handle)
{
	zval *value;
	bool root_changed;

	if (handle == NULL || !handle->has_original_value || handle->slot == NULL) {
		return true;
	}

	value = handle->slot;
	ZVAL_DEINDIRECT(value);
	ZVAL_DEREF(value);

	if (handle->original_root_type == IS_ARRAY || handle->original_root_type == IS_OBJECT) {
		root_changed = Z_TYPE_P(value) != handle->original_root_type ||
			Z_COUNTED_P(value) != handle->original_root
		;

		if (handle->snapshot_root_published && zend_opcache_static_cache_has_mutable_immediate_root(handle)) {
			return handle->mutation_dirty;
		}

		return handle->mutation_dirty || root_changed;
	}

	return handle->mutation_dirty || zend_opcache_static_cache_value_changed(value, &handle->original_value);
}

static bool zend_opcache_static_cache_pending_hash_is_root_array(
		zend_opcache_static_cache_static_slot_handle *handle,
		HashTable *ht)
{
	zval *value;

	if (handle == NULL || handle->slot == NULL || ht == NULL) {
		return false;
	}

	value = handle->slot;

	ZVAL_DEINDIRECT(value);
	ZVAL_DEREF(value);

	return Z_TYPE_P(value) == IS_ARRAY && Z_ARRVAL_P(value) == ht;
}

static bool zend_opcache_static_cache_value_contains_reachable_array(
		zval *value,
		HashTable *needle,
		HashTable *seen_arrays)
{
	zend_ulong key;
	zval *child;
	HashTable *ht;

	if (value == NULL || needle == NULL) {
		return false;
	}

	ZVAL_DEREF(value);
	if (Z_TYPE_P(value) != IS_ARRAY) {
		return false;
	}

	ht = Z_ARRVAL_P(value);
	if (ht == needle) {
		return true;
	}

	key = (zend_ulong) (uintptr_t) ht;
	if (zend_hash_index_exists(seen_arrays, key)) {
		return false;
	}

	zend_hash_index_add_empty_element(seen_arrays, key);
	ZEND_HASH_FOREACH_VAL(ht, child) {
		if (zend_opcache_static_cache_value_contains_reachable_array(child, needle, seen_arrays)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static bool zend_opcache_static_cache_pending_hash_is_current_reachable_array(
		zend_opcache_static_cache_static_slot_handle *handle,
		HashTable *ht)
{
	zval *value;
	HashTable seen_arrays;
	bool reachable;

	if (handle == NULL ||
		handle->slot == NULL ||
		ht == NULL ||
		!zend_opcache_static_cache_tracks_reachable_arrays(handle)
	) {
		return false;
	}

	value = handle->slot;
	ZVAL_DEINDIRECT(value);

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	reachable = zend_opcache_static_cache_value_contains_reachable_array(value, ht, &seen_arrays);
	zend_hash_destroy(&seen_arrays);

	return reachable;
}

static bool zend_opcache_static_cache_tracked_root_changed(zval *slot)
{
	zend_opcache_static_cache_static_slot_handle *handle;

	if (slot == NULL || zend_opcache_static_cache_tracked_roots == NULL) {
		return true;
	}

	handle = zend_hash_index_find_ptr(zend_opcache_static_cache_tracked_roots, (zend_ulong) (uintptr_t) slot);

	return zend_opcache_static_cache_static_slot_value_changed(handle);
}

static bool zend_opcache_static_cache_class_blob_changed(zend_opcache_static_cache_class_blob_handle *class_blob_handle)
{
	zend_opcache_static_cache_static_slot_handle *handle;

	if (class_blob_handle == NULL) {
		return false;
	}

	if (class_blob_handle->dirty) {
		return true;
	}

	if (zend_opcache_static_cache_tracked_roots == NULL) {
		return false;
	}

	ZEND_HASH_FOREACH_PTR(zend_opcache_static_cache_tracked_roots, handle) {
		if (handle != NULL &&
			handle->class_blob_handle == class_blob_handle &&
			zend_opcache_static_cache_static_slot_value_changed(handle)
		) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static bool zend_opcache_static_cache_register_captured_values(zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_ulong key;
	zend_object *obj;
	HashTable *ht;

	if (!zend_opcache_static_cache_capture_available || handle == NULL) {
		return false;
	}

	if (zend_opcache_static_cache_capture_arrays != NULL &&
		zend_opcache_static_cache_tracked_arrays != NULL &&
		zend_opcache_static_cache_tracks_reachable_arrays(handle)
	) {
		ZEND_HASH_FOREACH_NUM_KEY(zend_opcache_static_cache_capture_arrays, key) {
			ht = (HashTable *) (uintptr_t) key;
			zend_opcache_static_cache_track_dependency(zend_opcache_static_cache_tracked_arrays, key, handle);
			zend_opcache_static_cache_has_tracked_arrays = true;

			if (zend_opcache_static_cache_last_array_ht == ht) {
				zend_opcache_static_cache_last_array_ht = NULL;
				zend_opcache_static_cache_last_array_dependency = NULL;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_opcache_static_cache_capture_objects != NULL &&
		zend_opcache_static_cache_tracked_objects != NULL &&
		zend_opcache_static_cache_tracks_reachable_objects(handle)
	) {
		ZEND_HASH_FOREACH_NUM_KEY(zend_opcache_static_cache_capture_objects, key) {
			obj = (zend_object *) (uintptr_t) key;
			zend_opcache_static_cache_track_dependency(zend_opcache_static_cache_tracked_objects, key, handle);
			zend_opcache_static_cache_has_tracked_objects = true;

			if (zend_opcache_static_cache_last_object_obj == obj) {
				zend_opcache_static_cache_last_object_obj = NULL;
				zend_opcache_static_cache_last_object_dependency = NULL;
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_opcache_static_cache_update_mutation_hook_state();
	zend_opcache_static_cache_capture_clear();

	return true;
}

static void zend_opcache_static_cache_track_value_recursive(
	zval *value,
	zend_opcache_static_cache_static_slot_handle *handle,
	HashTable *seen_arrays,
	HashTable *seen_objects
)
{
	zend_ulong key;
	zend_string *property_name;
	zend_object *obj;
	zval *property_value, *source_value;
	HashTable *ht, *properties;

	if (value == NULL || handle == NULL) {
		return;
	}

	ZVAL_DEREF(value);
	switch (Z_TYPE_P(value)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(value);
			key = (zend_ulong) (uintptr_t) ht;

			if (zend_hash_index_exists(seen_arrays, key)) {
				return;
			}

			if (!zend_opcache_static_cache_tracks_reachable_arrays(handle)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_arrays, key);

			if (zend_opcache_static_cache_tracked_arrays != NULL) {
				zend_opcache_static_cache_track_dependency(zend_opcache_static_cache_tracked_arrays, key, handle);
				zend_opcache_static_cache_has_tracked_arrays = true;

				if (zend_opcache_static_cache_last_array_ht == ht) {
					zend_opcache_static_cache_last_array_ht = NULL;
					zend_opcache_static_cache_last_array_dependency = NULL;
				}
			}

			zend_opcache_static_cache_update_mutation_hook_state();

			ZEND_HASH_FOREACH_VAL(ht, property_value) {
				zend_opcache_static_cache_track_value_recursive(property_value, handle, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			return;
		case IS_OBJECT:
			obj = Z_OBJ_P(value);

			if (!zend_opcache_static_cache_tracks_reachable_objects(handle)) {
				/* Immediate attributes snapshot assigned objects at the root. Later object
				 * mutations require an explicit reassignment to publish. */
				return;
			}

			key = (zend_ulong) (uintptr_t) obj;

			if (zend_hash_index_exists(seen_objects, key)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_objects, key);

			if (zend_opcache_static_cache_tracked_objects != NULL) {
				zend_opcache_static_cache_track_dependency(zend_opcache_static_cache_tracked_objects, key, handle);
				zend_opcache_static_cache_has_tracked_objects = true;

				if (zend_opcache_static_cache_last_object_obj == obj) {
					zend_opcache_static_cache_last_object_obj = NULL;
					zend_opcache_static_cache_last_object_dependency = NULL;
				}
			}

			zend_opcache_static_cache_update_mutation_hook_state();

			properties = zend_get_properties_for(value, ZEND_PROP_PURPOSE_SERIALIZE);
			if (properties == NULL) {
				return;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					continue;
				}

				source_value =
					Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
				;

				zend_opcache_static_cache_track_value_recursive(source_value, handle, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);

			return;
		default:
			return;
	}
}

static bool zend_opcache_static_cache_prepare_memo_can_track_object(zend_class_entry *ce)
{
	return ce != NULL &&
		ce->type == ZEND_USER_CLASS &&
		zend_opcache_serializer_find_safe_direct_cache_base(ce) == NULL &&
		ce->create_object == NULL &&
		ce->__serialize == NULL &&
		ce->__unserialize == NULL &&
		!zend_hash_str_exists(&ce->function_table, ZEND_STRL("__sleep")) &&
		!zend_hash_str_exists(&ce->function_table, ZEND_STRL("__wakeup"))
	;
}

static bool zend_opcache_static_cache_prepare_memo_can_track_value_recursive(
		zval *value,
		HashTable *seen_arrays,
		HashTable *seen_objects)
{
	zend_ulong key;
	zend_string *property_name;
	zend_object *obj;
	zval *child, *property_value, *source_value;
	HashTable *ht, *properties;

	if (value == NULL) {
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
			ht = Z_ARRVAL_P(value);
			key = (zend_ulong) (uintptr_t) ht;
			if (zend_hash_index_exists(seen_arrays, key)) {
				return true;
			}

			zend_hash_index_add_empty_element(seen_arrays, key);

			ZEND_HASH_FOREACH_VAL(ht, child) {
				if (!zend_opcache_static_cache_prepare_memo_can_track_value_recursive(child, seen_arrays, seen_objects)) {
					return false;
				}
			} ZEND_HASH_FOREACH_END();

			return true;
		case IS_OBJECT:
			obj = Z_OBJ_P(value);
			if (!zend_opcache_static_cache_prepare_memo_can_track_object(obj->ce)) {
				return false;
			}

			key = (zend_ulong) (uintptr_t) obj;
			if (zend_hash_index_exists(seen_objects, key)) {
				return true;
			}

			zend_hash_index_add_empty_element(seen_objects, key);
			properties = zend_get_properties_for(value, ZEND_PROP_PURPOSE_SERIALIZE);
			if (properties == NULL) {
				return true;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					continue;
				}

				source_value =
					Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
				;

				if (!zend_opcache_static_cache_prepare_memo_can_track_value_recursive(source_value, seen_arrays, seen_objects)) {
					zend_release_properties(properties);
					return false;
				}
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);

			return true;
		default:
			return false;
	}
}

static void zend_opcache_static_cache_prepare_memo_track_value_recursive(
		zval *value,
		zend_opcache_static_cache_prepare_memo_entry *entry,
		HashTable *seen_arrays,
		HashTable *seen_objects)
{
	zend_object *obj;
	zend_ulong key;
	zend_string *property_name;
	zval *child, *property_value, *source_value;
	HashTable *ht, *properties;

	if (value == NULL || entry == NULL) {
		return;
	}

	ZVAL_DEREF(value);
	switch (Z_TYPE_P(value)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(value);
			key = (zend_ulong) (uintptr_t) ht;
			if (zend_hash_index_exists(seen_arrays, key)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_arrays, key);
			zend_opcache_static_cache_prepare_memo_track_dependency(zend_opcache_static_cache_prepare_memo_arrays, key, entry);
			zend_opcache_static_cache_has_prepare_memo_arrays = true;

			ZEND_HASH_FOREACH_VAL(ht, child) {
				zend_opcache_static_cache_prepare_memo_track_value_recursive(child, entry, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			return;
		case IS_OBJECT:
			obj = Z_OBJ_P(value);
			key = (zend_ulong) (uintptr_t) obj;
			if (zend_hash_index_exists(seen_objects, key)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_objects, key);
			zend_opcache_static_cache_prepare_memo_track_dependency(zend_opcache_static_cache_prepare_memo_objects, key, entry);
			zend_opcache_static_cache_has_prepare_memo_objects = true;
			properties = zend_get_properties_for(value, ZEND_PROP_PURPOSE_SERIALIZE);
			if (properties == NULL) {
				return;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					continue;
				}

				source_value =
					Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
				;

				zend_opcache_static_cache_prepare_memo_track_value_recursive(source_value, entry, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);

			return;
		default:
			return;
	}
}
static void zend_opcache_static_cache_register_tracked_value_ex(
		zval *slot,
		zend_string *cache_key,
		zend_opcache_static_cache_context *context,
		zend_opcache_static_cache_kind kind,
		zend_long ttl,
		zend_opcache_static_cache_class_blob_handle *class_blob_handle)
{
	zend_opcache_static_cache_static_slot_handle *handle;
	HashTable seen_arrays, seen_objects;

	if (slot == NULL || cache_key == NULL) {
		return;
	}

	handle = zend_opcache_static_cache_ensure_tracked_root(slot, cache_key, context, kind, ttl, class_blob_handle);
	if (handle == NULL) {
		return;
	}

	zend_opcache_static_cache_untrack_dependencies(handle);

	if (zend_opcache_static_cache_register_captured_values(handle)) {
		return;
	}

	if (!zend_opcache_static_cache_tracks_reachable_mutations(handle)) {
		return;
	}

	handle->request_deferred_publish = zend_opcache_static_cache_kind_defers_reachable_mutation_publish(handle->kind);

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);

	zend_opcache_static_cache_track_value_recursive(slot, handle, &seen_arrays, &seen_objects);
	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);
}

static void zend_opcache_static_cache_register_tracked_value(zval *slot, zend_string *cache_key, zend_opcache_static_cache_kind kind, zend_long ttl)
{
	zend_opcache_static_cache_register_tracked_value_ex(slot, cache_key, zend_opcache_static_cache_context_for_kind(kind), kind, ttl, NULL);
}

static void zend_opcache_static_cache_refresh_class_blob_root_originals(
		zend_opcache_static_cache_class_blob_handle *class_blob_handle)
{
	zend_opcache_static_cache_static_slot_handle *handle;

	if (class_blob_handle == NULL || zend_opcache_static_cache_tracked_roots == NULL) {
		return;
	}

	class_blob_handle->request_deferred_publish = false;

	ZEND_HASH_FOREACH_PTR(zend_opcache_static_cache_tracked_roots, handle) {
		if (handle == NULL || handle->class_blob_handle != class_blob_handle) {
			continue;
		}

		zend_opcache_static_cache_set_tracked_root_original(handle);
		handle->mutation_dirty = false;
		handle->request_deferred_publish = false;
		zend_opcache_static_cache_register_tracked_value_ex(
			handle->slot,
			handle->cache_key,
			handle->context,
			handle->kind,
			handle->ttl,
			class_blob_handle
		);
	} ZEND_HASH_FOREACH_END();
}

static bool zend_opcache_static_cache_sync_class_blob_function_static_entry(
		zend_opcache_static_cache_function_static_entry *entry)
{
	zval new_method_state, value_snapshot, *methods, *method_state, *value, *member;

	if (entry == NULL ||
		entry->slot == NULL ||
		entry->class_blob_handle == NULL ||
		entry->method_key == NULL ||
		entry->var_name == NULL
	) {
		return false;
	}

	methods = zend_opcache_static_cache_class_blob_ensure_section(entry->class_blob_handle, ZEND_STRL("methods"));
	method_state = zend_hash_find(Z_ARRVAL_P(methods), entry->method_key);
	if (method_state == NULL || Z_TYPE_P(method_state) != IS_ARRAY) {
		array_init(&new_method_state);
		method_state = zend_hash_update(Z_ARRVAL_P(methods), entry->method_key, &new_method_state);
	}

	value = entry->slot;
	ZVAL_DEINDIRECT(value);
	if (Z_TYPE_P(value) == IS_UNDEF) {
		return false;
	}

	member = zend_hash_find(Z_ARRVAL_P(method_state), entry->var_name);
	if (member != NULL && !zend_opcache_static_cache_value_changed(value, member)) {
		return false;
	}

	ZVAL_COPY_DEREF(&value_snapshot, value);
	zend_hash_update(Z_ARRVAL_P(method_state), entry->var_name, &value_snapshot);
	entry->class_blob_handle->dirty = true;

	return true;
}

static void zend_opcache_static_cache_sync_class_blob_function_static_handle(
		zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_opcache_static_cache_function_static_entry *entry;

	if (handle == NULL ||
		handle->slot == NULL ||
		handle->class_blob_handle == NULL ||
		!zend_opcache_static_cache_function_statics_initialized
	) {
		return;
	}

	entry = zend_hash_index_find_ptr(&zend_opcache_static_cache_function_statics, (zend_ulong) (uintptr_t) handle->slot);
	if (entry == NULL || entry->class_blob_handle != handle->class_blob_handle) {
		return;
	}

	zend_opcache_static_cache_sync_class_blob_function_static_entry(entry);
}

static void zend_opcache_static_cache_publish_immediate_root(zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_opcache_static_cache_context *previous_context;
	zval value_snapshot, *slot;
	bool published = false;

	if (handle == NULL ||
		!zend_opcache_static_cache_kind_publishes_immediately(handle->kind) ||
		!zend_opcache_static_cache_context_runtime(handle->context)->available ||
		!zend_opcache_static_cache_static_slot_value_changed(handle)
	) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(handle->context);
	if (zend_opcache_static_cache_wlock()) {
		if (zend_opcache_static_cache_header_init_locked()) {
			if (handle->class_blob_handle != NULL) {
				zend_opcache_static_cache_sync_class_blob_function_static_handle(handle);
				handle->class_blob_handle->dirty = true;
				published = zend_opcache_static_cache_publish_class_blob_handle_locked(handle->class_blob_handle);
			} else {
				slot = handle->slot;
				ZVAL_DEINDIRECT(slot);
				if (Z_TYPE_P(slot) == IS_UNDEF) {
					published = zend_opcache_static_cache_delete_locked(handle->cache_key);
				} else {
					ZVAL_COPY_DEREF(&value_snapshot, slot);
					published = zend_opcache_static_cache_store_locked(handle->cache_key, &value_snapshot, handle->ttl, false, true);
					zval_ptr_dtor(&value_snapshot);
				}
			}
		}

		zend_opcache_static_cache_unlock();
	}

	zend_opcache_static_cache_restore_context(previous_context);

	if (!published) {
		return;
	}

	if (handle->class_blob_handle != NULL) {
		zend_opcache_static_cache_refresh_class_blob_root_originals(handle->class_blob_handle);
	} else {
		zend_opcache_static_cache_set_tracked_root_original(handle);
		handle->snapshot_root_published = zend_opcache_static_cache_has_mutable_immediate_root(handle);
		handle->mutation_dirty = false;
		handle->request_deferred_publish = false;
		zend_opcache_static_cache_register_tracked_value(handle->slot, handle->cache_key, handle->kind, handle->ttl);
	}
}

static void zend_opcache_static_cache_publish_class_blob_fast(zend_opcache_static_cache_class_blob_handle *class_blob_handle)
{
	zend_opcache_static_cache_context *previous_context;
	bool published = false;

	if (class_blob_handle == NULL ||
		!zend_opcache_static_cache_kind_publishes_immediately(class_blob_handle->kind) ||
		!class_blob_handle->initialized ||
		!zend_opcache_static_cache_context_runtime(class_blob_handle->context)->available ||
		!zend_opcache_static_cache_class_blob_changed(class_blob_handle)
	) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(class_blob_handle->context);
	if (zend_opcache_static_cache_wlock()) {
		if (zend_opcache_static_cache_header_init_locked()) {
			/* Immediate class blobs publish root assignments immediately. StableStatic
			 * array graphs are re-registered after publication so later array writes
			 * also fail or succeed at the write site. */
			class_blob_handle->dirty = true;
			published = zend_opcache_static_cache_publish_class_blob_handle_locked(class_blob_handle);
		}
		zend_opcache_static_cache_unlock();
	}

	zend_opcache_static_cache_restore_context(previous_context);

	if (published) {
		zend_opcache_static_cache_refresh_class_blob_root_originals(class_blob_handle);
	}
}

static bool zend_opcache_static_cache_install_class_blob_property_refs(
		zend_opcache_static_cache_class_blob_handle *handle)
{
	zend_string *property_name;
	zend_property_info *property_info;
	zval snapshot, *properties, *slot, *member;

	if (handle == NULL || handle->ce == NULL || CE_STATIC_MEMBERS(handle->ce) == NULL) {
		return false;
	}

	properties = zend_opcache_static_cache_class_blob_ensure_section(handle, ZEND_STRL("properties"));
	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&handle->ce->properties_info, property_name, property_info) {
		if (property_name == NULL || !zend_opcache_static_cache_class_blob_applies_to_property(handle->ce, property_info)) {
			continue;
		}

		slot = CE_STATIC_MEMBERS(handle->ce) + property_info->offset;
		ZVAL_DEINDIRECT(slot);
		member = zend_hash_find(Z_ARRVAL_P(properties), property_name);
		if (member == NULL) {
			ZVAL_COPY_DEREF(&snapshot, slot);
			member = zend_hash_update(Z_ARRVAL_P(properties), property_name, &snapshot);
			handle->dirty = true;
		}

		zend_opcache_static_cache_bind_slot_to_member(slot, member);
		zend_opcache_static_cache_register_tracked_value_ex(slot, handle->cache_key, handle->context, handle->kind, handle->ttl, handle);
	} ZEND_HASH_FOREACH_END();

	return true;
}

static bool zend_opcache_static_cache_bind_class_blob_method_refs(
		zend_opcache_static_cache_class_blob_handle *handle,
		zend_op_array *op_array,
		HashTable *static_variables)
{
	zend_string *method_key, *var_name;
	zval new_method_state, snapshot, *methods, *method_state, *slot, *member;

	if (handle == NULL || op_array == NULL || static_variables == NULL) {
		return false;
	}

	methods = zend_opcache_static_cache_class_blob_ensure_section(handle, ZEND_STRL("methods"));
	method_key = zend_opcache_static_cache_class_blob_method_key(op_array);
	if (method_key == NULL) {
		return false;
	}

	method_state = zend_hash_find(Z_ARRVAL_P(methods), method_key);
	if (method_state == NULL || Z_TYPE_P(method_state) != IS_ARRAY) {
		array_init(&new_method_state);
		method_state = zend_hash_update(Z_ARRVAL_P(methods), method_key, &new_method_state);
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(static_variables, var_name, slot) {
		if (var_name == NULL) {
			continue;
		}

		zend_opcache_static_cache_track_class_blob_function_static_slot(slot, handle, method_key, var_name);

		member = zend_hash_find(Z_ARRVAL_P(method_state), var_name);
		if (member == NULL) {
			if (Z_TYPE_P(slot) == IS_NULL) {
				handle->dirty = true;
				zend_opcache_static_cache_register_tracked_value_ex(slot, handle->cache_key, handle->context, handle->kind, handle->ttl, handle);

				continue;
			} else {
				ZVAL_COPY_DEREF(&snapshot, slot);
				member = zend_hash_update(Z_ARRVAL_P(method_state), var_name, &snapshot);

				handle->dirty = true;
			}
		}

		zend_opcache_static_cache_bind_slot_to_member(slot, member);
		zend_opcache_static_cache_register_tracked_value_ex(slot, handle->cache_key, handle->context, handle->kind, handle->ttl, handle);
	} ZEND_HASH_FOREACH_END();

	zend_string_release(method_key);

	return true;
}

static void zend_opcache_static_cache_init_tracking(void)
{
	zend_opcache_static_cache_has_tracked_arrays = false;
	zend_opcache_static_cache_has_tracked_objects = false;
	zend_opcache_static_cache_has_prepare_memo_arrays = false;
	zend_opcache_static_cache_has_prepare_memo_objects = false;
	zend_opcache_static_cache_capture_active = false;
	zend_opcache_static_cache_capture_clear();
	zend_opcache_static_cache_last_array_ht = NULL;
	zend_opcache_static_cache_last_array_dependency = NULL;
	zend_opcache_static_cache_last_object_obj = NULL;
	zend_opcache_static_cache_last_object_dependency = NULL;
	zend_opcache_static_cache_update_mutation_hook_state();

	if (zend_opcache_static_cache_tracked_roots == NULL) {
		zend_opcache_static_cache_tracked_roots = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_tracked_roots, 8, NULL, zend_opcache_static_cache_tracked_root_dtor, 0);
	}

	if (zend_opcache_static_cache_tracked_references == NULL) {
		zend_opcache_static_cache_tracked_references = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_tracked_references, 8, NULL, NULL, 0);
	}

	if (zend_opcache_static_cache_tracked_arrays == NULL) {
		zend_opcache_static_cache_tracked_arrays = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_tracked_arrays, 8, NULL, zend_opcache_static_cache_tracked_dependency_dtor, 0);
	}

	if (zend_opcache_static_cache_tracked_objects == NULL) {
		zend_opcache_static_cache_tracked_objects = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_tracked_objects, 8, NULL, zend_opcache_static_cache_tracked_dependency_dtor, 0);
	}

	zend_opcache_static_cache_reset_pending_mutation();
}

static void zend_opcache_static_cache_destroy_tracking(void)
{
	zend_opcache_static_cache_reset_pending_mutation();
	zend_opcache_static_cache_has_tracked_arrays = false;
	zend_opcache_static_cache_has_tracked_objects = false;
	zend_opcache_static_cache_has_prepare_memo_arrays = false;
	zend_opcache_static_cache_has_prepare_memo_objects = false;
	zend_opcache_static_cache_capture_active = false;
	zend_opcache_static_cache_capture_available = false;
	zend_opcache_static_cache_last_array_ht = NULL;
	zend_opcache_static_cache_last_array_dependency = NULL;
	zend_opcache_static_cache_last_object_obj = NULL;
	zend_opcache_static_cache_last_object_dependency = NULL;
	zend_opcache_static_cache_update_mutation_hook_state();

	if (zend_opcache_static_cache_tracked_roots != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_tracked_roots);
		efree(zend_opcache_static_cache_tracked_roots);
		zend_opcache_static_cache_tracked_roots = NULL;
	}

	if (zend_opcache_static_cache_tracked_references != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_tracked_references);
		efree(zend_opcache_static_cache_tracked_references);
		zend_opcache_static_cache_tracked_references = NULL;
	}

	if (zend_opcache_static_cache_tracked_arrays != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_tracked_arrays);
		efree(zend_opcache_static_cache_tracked_arrays);
		zend_opcache_static_cache_tracked_arrays = NULL;
	}

	if (zend_opcache_static_cache_tracked_objects != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_tracked_objects);
		efree(zend_opcache_static_cache_tracked_objects);
		zend_opcache_static_cache_tracked_objects = NULL;
	}

	if (zend_opcache_static_cache_prepare_memo_entries != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_prepare_memo_entries);
		efree(zend_opcache_static_cache_prepare_memo_entries);
		zend_opcache_static_cache_prepare_memo_entries = NULL;
	}

	if (zend_opcache_static_cache_prepare_memo_array_roots != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_prepare_memo_array_roots);
		efree(zend_opcache_static_cache_prepare_memo_array_roots);
		zend_opcache_static_cache_prepare_memo_array_roots = NULL;
	}

	if (zend_opcache_static_cache_prepare_memo_object_roots != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_prepare_memo_object_roots);
		efree(zend_opcache_static_cache_prepare_memo_object_roots);
		zend_opcache_static_cache_prepare_memo_object_roots = NULL;
	}

	if (zend_opcache_static_cache_prepare_memo_arrays != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_prepare_memo_arrays);
		efree(zend_opcache_static_cache_prepare_memo_arrays);
		zend_opcache_static_cache_prepare_memo_arrays = NULL;
	}

	if (zend_opcache_static_cache_prepare_memo_objects != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_prepare_memo_objects);
		efree(zend_opcache_static_cache_prepare_memo_objects);
		zend_opcache_static_cache_prepare_memo_objects = NULL;
	}

	if (zend_opcache_static_cache_capture_arrays != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_capture_arrays);
		efree(zend_opcache_static_cache_capture_arrays);
		zend_opcache_static_cache_capture_arrays = NULL;
	}

	if (zend_opcache_static_cache_capture_objects != NULL) {
		zend_hash_destroy(zend_opcache_static_cache_capture_objects);
		efree(zend_opcache_static_cache_capture_objects);
		zend_opcache_static_cache_capture_objects = NULL;
	}
}

static void zend_opcache_static_cache_mark_dirty_class_blob_handles(void)
{
	zend_opcache_static_cache_static_slot_handle *handle;

	if (zend_opcache_static_cache_tracked_roots == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_PTR(zend_opcache_static_cache_tracked_roots, handle) {
		if (handle == NULL ||
			handle->class_blob_handle == NULL ||
			!handle->has_original_value ||
			handle->slot == NULL
		) {
			continue;
		}

		if (zend_opcache_static_cache_static_slot_value_changed(handle)) {
			handle->class_blob_handle->dirty = true;
		}
	} ZEND_HASH_FOREACH_END();
}

static void zend_opcache_static_cache_flush_pending(void)
{
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_opcache_static_cache_context *previous_context;
	zval value_snapshot;
	bool should_store, published = false;

	if (!zend_opcache_static_cache_pending_mutation_state.dirty ||
		zend_opcache_static_cache_pending_mutation_state.handle == NULL
	) {
		zend_opcache_static_cache_reset_pending_mutation();

		return;
	}

	handle = zend_opcache_static_cache_pending_mutation_state.handle;
	if (!zend_opcache_static_cache_context_runtime(handle->context)->available) {
		zend_opcache_static_cache_reset_pending_mutation();

		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(handle->context);
	if (zend_opcache_static_cache_wlock()) {
		if (zend_opcache_static_cache_header_init_locked()) {
			if (handle->class_blob_handle != NULL) {
				zend_opcache_static_cache_sync_class_blob_function_static_handle(handle);
				handle->class_blob_handle->dirty = true;
				published = zend_opcache_static_cache_publish_class_blob_handle_locked(handle->class_blob_handle);
			} else {
				ZVAL_COPY_DEREF(&value_snapshot, handle->slot);
				should_store = Z_TYPE(value_snapshot) != IS_UNDEF;
				if (should_store) {
					published = zend_opcache_static_cache_store_locked(handle->cache_key, &value_snapshot, handle->ttl, false, true);
				} else {
					published = zend_opcache_static_cache_delete_locked(handle->cache_key);
				}

				zval_ptr_dtor(&value_snapshot);
			}
		}

		zend_opcache_static_cache_unlock();
	}

	zend_opcache_static_cache_restore_context(previous_context);

	if (handle->class_blob_handle != NULL) {
		if (published && zend_opcache_static_cache_kind_publishes_immediately(handle->kind)) {
			zend_opcache_static_cache_refresh_class_blob_root_originals(handle->class_blob_handle);
		} else {
			zend_opcache_static_cache_register_tracked_value_ex(handle->slot, handle->cache_key, handle->context, handle->kind, handle->ttl, handle->class_blob_handle);
		}
	} else {
		if (published && zend_opcache_static_cache_kind_publishes_immediately(handle->kind)) {
			zend_opcache_static_cache_set_tracked_root_original(handle);
			handle->snapshot_root_published = zend_opcache_static_cache_has_mutable_immediate_root(handle);
			handle->mutation_dirty = false;
			handle->request_deferred_publish = false;
		}
		zend_opcache_static_cache_register_tracked_value(handle->slot, handle->cache_key, handle->kind, handle->ttl);
	}

	zend_opcache_static_cache_reset_pending_mutation();
}

static bool zend_opcache_static_cache_defer_mutation_publish(zend_opcache_static_cache_static_slot_handle *handle)
{
	if (handle == NULL || !handle->request_deferred_publish) {
		return false;
	}

	if (zend_opcache_static_cache_pending_mutation_state.handle != NULL &&
		zend_opcache_static_cache_pending_mutation_state.handle != handle
	) {
		zend_opcache_static_cache_flush_pending();
	}

	handle->mutation_dirty = true;

	return true;
}

static bool zend_opcache_static_cache_begin_mutation(
		zend_opcache_static_cache_static_slot_handle *handle,
		HashTable *ht)
{
	bool root_array;

	if (handle == NULL) {
		return false;
	}

	if (zend_opcache_static_cache_pending_mutation_state.handle != NULL &&
		zend_opcache_static_cache_pending_mutation_state.handle != handle
	) {
		zend_opcache_static_cache_flush_pending();
	}

	root_array = zend_opcache_static_cache_pending_hash_is_root_array(handle, ht);
	if (zend_opcache_static_cache_pending_mutation_state.handle == handle &&
		zend_opcache_static_cache_pending_mutation_state.dirty
	) {
		zend_opcache_static_cache_pending_mutation_state.depth++;
		zend_opcache_static_cache_pending_mutation_state.root_array &= root_array;

		return true;
	}

	zend_opcache_static_cache_pending_mutation_state.handle = handle;
	zend_opcache_static_cache_pending_mutation_state.depth = 1;
	zend_opcache_static_cache_pending_mutation_state.dirty = true;
	zend_opcache_static_cache_pending_mutation_state.root_array = root_array;

	return true;
}

static bool zend_opcache_static_cache_defer_dependency_publish(
		zend_opcache_static_cache_tracked_dependency *dependency)
{
	zend_opcache_static_cache_static_slot_handle *handle;
	bool deferred = false;

	if (dependency == NULL) {
		return false;
	}

	if (dependency->handles == NULL) {
		return zend_opcache_static_cache_defer_mutation_publish(dependency->single_handle);
	}

	ZEND_HASH_FOREACH_PTR(dependency->handles, handle) {
		if (zend_opcache_static_cache_defer_mutation_publish(handle)) {
			deferred = true;
		}
	} ZEND_HASH_FOREACH_END();

	return deferred;
}

static zend_opcache_static_cache_static_slot_handle *zend_opcache_static_cache_tracked_dependency_first_handle(
		zend_opcache_static_cache_tracked_dependency *dependency)
{
	zend_opcache_static_cache_static_slot_handle *handle;

	if (dependency == NULL) {
		return NULL;
	}

	if (dependency->handles == NULL) {
		return dependency->single_handle;
	}

	ZEND_HASH_FOREACH_PTR(dependency->handles, handle) {
		return handle;
	} ZEND_HASH_FOREACH_END();

	return NULL;
}

static bool zend_opcache_static_cache_method_has_keys(zend_class_entry *ce)
{
	zend_opcache_static_cache_kind kind;
	zend_function *function;
	zend_op_array *op_array;

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, function) {
		if (function == NULL || function->type != ZEND_USER_FUNCTION) {
			continue;
		}

		op_array = &function->op_array;
		kind = zend_opcache_static_cache_op_array_kind(op_array);
		if (kind != ZEND_OPCACHE_STATIC_CACHE_NONE &&
			zend_opcache_static_cache_context_for_kind(kind) == zend_opcache_static_cache_active_context()
		) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

bool zend_opcache_static_cache_class_has_keys(zend_class_entry *ce)
{
	zend_opcache_static_cache_kind kind;
	zend_string *property_name;
	zend_property_info *property_info;

	if (ce == NULL || ce->name == NULL) {
		return false;
	}

	if (zend_opcache_static_cache_class_blob_enabled(ce)) {
		kind = zend_opcache_static_cache_class_blob_kind(ce);

		return zend_opcache_static_cache_context_for_kind(kind) == zend_opcache_static_cache_active_context();
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
		if (property_name == NULL) {
			continue;
		}

		kind = zend_opcache_static_cache_static_property_kind(ce, property_info);
		if (kind != ZEND_OPCACHE_STATIC_CACHE_NONE &&
			zend_opcache_static_cache_context_for_kind(kind) == zend_opcache_static_cache_active_context()
		) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return zend_opcache_static_cache_method_has_keys(ce);
}

static void zend_opcache_static_cache_delete_method_keys_locked(zend_class_entry *ce)
{
	zend_opcache_static_cache_kind kind;
	zend_function *function;
	zend_op_array *op_array;
	zend_string *var_name, *cache_key;
	zval *slot;

	ZEND_HASH_MAP_FOREACH_PTR(&ce->function_table, function) {
		if (function == NULL || function->type != ZEND_USER_FUNCTION) {
			continue;
		}

		op_array = &function->op_array;
		kind = zend_opcache_static_cache_op_array_kind(op_array);
		if (kind == ZEND_OPCACHE_STATIC_CACHE_NONE ||
			zend_opcache_static_cache_context_for_kind(kind) != zend_opcache_static_cache_active_context()
		) {
			continue;
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL(op_array->static_variables, var_name, slot) {
			(void) slot;

			if (var_name == NULL) {
				continue;
			}

			cache_key = zend_opcache_static_cache_function_variable_key(op_array, var_name, kind);
			if (cache_key == NULL) {
				continue;
			}

			zend_opcache_static_cache_delete_locked(cache_key);
			zend_string_release(cache_key);
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
}

void zend_opcache_static_cache_delete_class_keys_locked(zend_class_entry *ce)
{
	zend_opcache_static_cache_kind kind;
	zend_string *property_name, *cache_key;
	zend_property_info *property_info;

	if (ce == NULL || ce->name == NULL) {
		return;
	}

	if (zend_opcache_static_cache_class_blob_enabled(ce)) {
		kind = zend_opcache_static_cache_class_blob_kind(ce);

		if (zend_opcache_static_cache_context_for_kind(kind) != zend_opcache_static_cache_active_context()) {
			return;
		}

		cache_key = zend_opcache_static_cache_class_blob_key(ce, kind);
		if (cache_key != NULL) {
			zend_opcache_static_cache_delete_locked(cache_key);
			zend_string_release(cache_key);
		}

		return;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
		if (property_name == NULL) {
			continue;
		}

		kind = zend_opcache_static_cache_static_property_kind(ce, property_info);
		if (kind == ZEND_OPCACHE_STATIC_CACHE_NONE ||
			zend_opcache_static_cache_context_for_kind(kind) != zend_opcache_static_cache_active_context()
		) {
			continue;
		}

		cache_key = zend_opcache_static_cache_static_property_key(ce, property_name, kind);
		if (cache_key == NULL) {
			continue;
		}

		zend_opcache_static_cache_delete_locked(cache_key);
		zend_string_release(cache_key);
	} ZEND_HASH_FOREACH_END();

	zend_opcache_static_cache_delete_method_keys_locked(ce);
}

static bool zend_opcache_static_cache_class_access_filter(
		zend_class_entry *ce,
		zend_opcache_static_cache_class_blob_handle **blob_out)
{
	zend_opcache_static_cache_class_blob_handle *blob;
	zend_ulong key;
	zend_string *property_name;
	zend_property_info *property_info;

	*blob_out = NULL;
	if (UNEXPECTED(ce->name == NULL)) {
		return false;
	}

	blob = zend_opcache_static_cache_find_class_blob_handle(ce);
	if (blob != NULL) {
		*blob_out = blob;

		return true;
	}

	key = (zend_ulong) (uintptr_t) ce;
	if (zend_opcache_static_cache_ignored_classes_initialized &&
		zend_hash_index_exists(&zend_opcache_static_cache_ignored_classes, key)
	) {
		return false;
	}

	if (zend_opcache_static_cache_class_blob_enabled(ce)) {
		return true;
	}

	if (zend_opcache_static_cache_attribute_classes_initialized &&
		zend_hash_index_exists(&zend_opcache_static_cache_attribute_classes, key)
	) {
		return false;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
		if (property_name != NULL && zend_opcache_static_cache_applies_to_static_property(ce, property_info)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	if (zend_opcache_static_cache_ignored_classes_initialized) {
		zend_hash_index_add_empty_element(&zend_opcache_static_cache_ignored_classes, key);
	}

	return false;
}

static void zend_opcache_static_cache_publish_stable_static_properties_fast(zend_class_entry *ce)
{
	zend_opcache_static_cache_context *context, *previous_context;
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_opcache_static_cache_kind kind;
	zend_ulong class_key;
	zend_long ttl;
	zend_string *property_name, *cache_key;
	zend_property_info *property_info;
	zval *slot;
	bool published;

	if (!zend_opcache_static_cache_attribute_classes_initialized || ce->name == NULL) {
		return;
	}

	class_key = (zend_ulong) (uintptr_t) ce;
	if (!zend_hash_index_exists(&zend_opcache_static_cache_attribute_classes, class_key)) {
		return;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
		if (property_name == NULL) {
			continue;
		}

		kind = zend_opcache_static_cache_static_property_kind_and_ttl(ce, property_info, &ttl);
		if (!zend_opcache_static_cache_kind_publishes_immediately(kind)) {
			continue;
		}

		context = zend_opcache_static_cache_context_for_kind(kind);
		if (!zend_opcache_static_cache_context_runtime(context)->available) {
			continue;
		}

		cache_key = zend_opcache_static_cache_static_property_key(ce, property_name, kind);
		if (cache_key == NULL) {
			continue;
		}

		slot = CE_STATIC_MEMBERS(ce) + property_info->offset;
		ZVAL_DEINDIRECT(slot);
		handle = zend_opcache_static_cache_ensure_tracked_root(slot, cache_key, context, kind, ttl, NULL);
		if (!zend_opcache_static_cache_static_slot_value_changed(handle)) {
			zend_string_release(cache_key);

			continue;
		}

		published = false;
		previous_context = zend_opcache_static_cache_activate_context(context);

		if (zend_opcache_static_cache_wlock()) {
			if (zend_opcache_static_cache_header_init_locked()) {
				/* Immediate property assignments publish immediately so later
				 * mutations of the assigned graph do not change the snapshot. */
				if (Z_TYPE_P(slot) == IS_UNDEF) {
					published = zend_opcache_static_cache_delete_locked(cache_key);
				} else {
					published = zend_opcache_static_cache_store_locked(cache_key, slot, ttl, false, true);
				}
			}

			zend_opcache_static_cache_unlock();
		}

		zend_opcache_static_cache_restore_context(previous_context);

		if (published) {
			zend_opcache_static_cache_set_tracked_root_original(handle);
			if (handle != NULL) {
				handle->snapshot_root_published = zend_opcache_static_cache_has_mutable_immediate_root(handle);
				handle->mutation_dirty = false;
				handle->request_deferred_publish = false;
			}
		}

		zend_opcache_static_cache_register_tracked_value(slot, cache_key, kind, ttl);
		zend_string_release(cache_key);
	} ZEND_HASH_FOREACH_END();
}

static bool zend_opcache_static_cache_context_has_publish_work(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_opcache_static_cache_static_slot_handle *handle;

	if (zend_opcache_static_cache_class_blob_handles_initialized) {
		ZEND_HASH_FOREACH_PTR(&zend_opcache_static_cache_class_blob_handles, class_blob_handle) {
			if (class_blob_handle != NULL &&
				class_blob_handle->context == context &&
				zend_opcache_static_cache_class_blob_changed(class_blob_handle)
			) {
				return true;
			}
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_opcache_static_cache_tracked_roots != NULL) {
		ZEND_HASH_FOREACH_PTR(zend_opcache_static_cache_tracked_roots, handle) {
			if (handle != NULL && handle->context == context && zend_opcache_static_cache_static_slot_value_changed(handle)) {
				return true;
			}
		} ZEND_HASH_FOREACH_END();
	}

	return false;
}

static void zend_opcache_static_cache_publish_context(zend_opcache_static_cache_context *context)
{
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_function_static_entry *entry;
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_opcache_static_cache_kind kind;
	zend_long ttl;
	zend_string *property_name, *cache_key;
	zend_class_entry *ce;
	zend_property_info *property_info;
	zval value_snapshot, *value, *slot;

	if (!zend_opcache_static_cache_context_runtime(context)->available) {
		return;
	}

	if (zend_opcache_static_cache_publish_skipped(context)) {
		return;
	}

	if (!zend_opcache_static_cache_context_has_publish_work(context)) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);
	if (!zend_opcache_static_cache_wlock()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_static_cache_header_init_locked()) {
		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	zend_opcache_static_cache_mark_dirty_class_blob_handles();

	if (zend_opcache_static_cache_function_statics_initialized) {
		ZEND_HASH_FOREACH_PTR(&zend_opcache_static_cache_function_statics, entry) {
			if (entry == NULL || entry->slot == NULL || entry->class_blob_handle == NULL || entry->context != context) {
				continue;
			}

			zend_opcache_static_cache_sync_class_blob_function_static_entry(entry);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_opcache_static_cache_class_blob_handles_initialized) {
		ZEND_HASH_FOREACH_PTR(&zend_opcache_static_cache_class_blob_handles, class_blob_handle) {
			if (class_blob_handle == NULL || class_blob_handle->context != context) {
				continue;
			}

			zend_opcache_static_cache_publish_class_blob_handle_locked(class_blob_handle);
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_opcache_static_cache_attribute_classes_initialized) {
		ZEND_HASH_FOREACH_PTR(&zend_opcache_static_cache_attribute_classes, ce) {
			if (ce == NULL || ce->name == NULL || CE_STATIC_MEMBERS(ce) == NULL) {
				continue;
			}

			ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
				if (property_name == NULL) {
					continue;
				}

				kind = zend_opcache_static_cache_static_property_kind_and_ttl(ce, property_info, &ttl);
				if (kind == ZEND_OPCACHE_STATIC_CACHE_NONE || zend_opcache_static_cache_context_for_kind(kind) != context) {
					continue;
				}

				slot = CE_STATIC_MEMBERS(ce) + property_info->offset;
				ZVAL_DEINDIRECT(slot);
				if (!zend_opcache_static_cache_tracked_root_changed(slot)) {
					continue;
				}

				cache_key = zend_opcache_static_cache_static_property_key(ce, property_name, kind);
				if (cache_key == NULL) {
					continue;
				}

				if (Z_TYPE_P(slot) == IS_UNDEF) {
					zend_opcache_static_cache_delete_locked(cache_key);
				} else {
					zend_opcache_static_cache_store_locked(cache_key, slot, ttl, false, true);
				}

				zend_string_release(cache_key);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}

	if (zend_opcache_static_cache_function_statics_initialized) {
		ZEND_HASH_FOREACH_PTR(&zend_opcache_static_cache_function_statics, entry) {
			if (entry == NULL ||
				entry->slot == NULL ||
				entry->class_blob_handle != NULL ||
				entry->context != context
			) {
				continue;
			}

			value = entry->slot;
			if (!zend_opcache_static_cache_tracked_root_changed(value)) {
				continue;
			}

			ZVAL_DEINDIRECT(value);
			ZVAL_COPY_DEREF(&value_snapshot, value);
			if (Z_TYPE(value_snapshot) == IS_UNDEF) {
				zend_opcache_static_cache_delete_locked(entry->cache_key);
			} else {
				zend_opcache_static_cache_store_locked(entry->cache_key, &value_snapshot, entry->ttl, false, true);
			}

			zval_ptr_dtor(&value_snapshot);
		} ZEND_HASH_FOREACH_END();
	}

	zend_opcache_static_cache_unlock();

	zend_opcache_static_cache_restore_context(previous_context);
}

static void zend_opcache_static_cache_capture_decoded_hash_values(
	HashTable *values,
	HashTable *seen_arrays,
	HashTable *seen_objects
)
{
	zval *child, *source_value;

	if (values == NULL) {
		return;
	}

	ZEND_HASH_FOREACH_VAL(values, child) {
		source_value = Z_TYPE_P(child) == IS_INDIRECT ? Z_INDIRECT_P(child) : child;
		if (Z_TYPE_P(source_value) != IS_UNDEF) {
			zend_opcache_static_cache_capture_decoded_reachable_value_ex(source_value, seen_arrays, seen_objects);
		}
	} ZEND_HASH_FOREACH_END();
}

static bool zend_opcache_static_cache_capture_decoded_safe_direct_object(
	zval *value,
	HashTable *seen_arrays,
	HashTable *seen_objects
)
{
	zend_opcache_static_cache_safe_direct_state_serialize_func_t serialize_func;
	zend_class_entry *ce, *base_ce = NULL;
	zval state;
	HashTable *properties;

	ce = Z_OBJCE_P(value);
	base_ce = zend_opcache_serializer_find_safe_direct_cache_base(ce);
	if (base_ce == NULL ||
		zend_opcache_serializer_has_safe_direct_cache_overrides(ce, base_ce)
	) {
		return false;
	}

	serialize_func = zend_opcache_static_cache_safe_direct_state_serialize_func(ce);
	if (serialize_func == NULL) {
		return false;
	}

	/* Internal safe-direct objects may reject ZEND_PROP_PURPOSE_SERIALIZE
	 * because their cache state is exposed through explicit handlers. */
	ZVAL_UNDEF(&state);
	if (serialize_func(value, &state) && Z_TYPE(state) == IS_ARRAY) {
		zend_opcache_static_cache_capture_decoded_hash_values(Z_ARRVAL(state), seen_arrays, seen_objects);
	}

	if (!Z_ISUNDEF(state)) {
		zval_ptr_dtor(&state);
	}

	if (EG(exception)) {
		return true;
	}

	if (!zend_opcache_static_cache_safe_direct_state_includes_properties(ce)) {
		properties = zend_std_get_properties(Z_OBJ_P(value));
		zend_opcache_static_cache_capture_decoded_hash_values(properties, seen_arrays, seen_objects);
	}

	return true;
}

static void zend_opcache_static_cache_capture_decoded_reachable_value_ex(
	zval *value,
	HashTable *seen_arrays,
	HashTable *seen_objects
)
{
	zend_ulong key;
	zend_string *property_name;
	zend_object *obj;
	zval *child, *property_value, *source_value;
	HashTable *ht, *properties;

	if (!zend_opcache_static_cache_capture_active || value == NULL) {
		return;
	}

	if (zend_opcache_static_cache_capture_handle != NULL &&
		zend_opcache_static_cache_capture_handle->kind == ZEND_OPCACHE_STATIC_CACHE_STABLE
	) {
		return;
	}

	ZVAL_DEREF(value);
	switch (Z_TYPE_P(value)) {
		case IS_ARRAY:
			ht = Z_ARRVAL_P(value);
			key = (zend_ulong) (uintptr_t) ht;

			if (zend_hash_index_exists(seen_arrays, key)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_arrays, key);
			zend_opcache_static_cache_capture_decoded_value(value);

			ZEND_HASH_FOREACH_VAL(ht, child) {
				zend_opcache_static_cache_capture_decoded_reachable_value_ex(child, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			return;
		case IS_OBJECT:
			obj = Z_OBJ_P(value);
			key = (zend_ulong) (uintptr_t) obj;

			if (zend_hash_index_exists(seen_objects, key)) {
				return;
			}

			zend_hash_index_add_empty_element(seen_objects, key);
			zend_opcache_static_cache_capture_decoded_value(value);
			if (zend_opcache_static_cache_capture_handle != NULL &&
				!zend_opcache_static_cache_tracks_reachable_objects(zend_opcache_static_cache_capture_handle)
			) {
				return;
			}

			if (zend_opcache_static_cache_capture_decoded_safe_direct_object(value, seen_arrays, seen_objects)) {
				return;
			}

			properties = zend_get_properties_for(value, ZEND_PROP_PURPOSE_SERIALIZE);
			if (properties == NULL) {
				return;
			}

			ZEND_HASH_FOREACH_STR_KEY_VAL(properties, property_name, property_value) {
				if (property_name == NULL) {
					continue;
				}

				source_value =
					Z_TYPE_P(property_value) == IS_INDIRECT
						? Z_INDIRECT_P(property_value)
						: property_value
				;

				zend_opcache_static_cache_capture_decoded_reachable_value_ex(source_value, seen_arrays, seen_objects);
			} ZEND_HASH_FOREACH_END();

			zend_release_properties(properties);

			return;
		default:
			return;
	}
}

static bool zend_opcache_static_cache_hash_mutation(HashTable *ht, bool publish)
{
	zend_opcache_static_cache_tracked_dependency *dependency;
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_opcache_static_cache_prepare_memo_dependency *memo_dependency;

	if (publish) {
		if (zend_opcache_static_cache_pending_mutation_state.dirty &&
			zend_opcache_static_cache_pending_mutation_state.handle != NULL &&
			zend_opcache_static_cache_kind_publishes_immediately(
				zend_opcache_static_cache_pending_mutation_state.handle->kind) &&
			zend_opcache_static_cache_tracks_reachable_arrays(
				zend_opcache_static_cache_pending_mutation_state.handle) &&
			!zend_opcache_static_cache_pending_hash_is_current_reachable_array(
				zend_opcache_static_cache_pending_mutation_state.handle, ht)
		) {
			zend_opcache_static_cache_reset_pending_mutation();
		} else {
			zend_opcache_static_cache_flush_pending();
		}

		return false;
	}

	if (ht != NULL && zend_opcache_static_cache_has_prepare_memo_arrays && zend_opcache_static_cache_prepare_memo_arrays != NULL) {
		memo_dependency = zend_hash_index_find_ptr(zend_opcache_static_cache_prepare_memo_arrays, (zend_ulong) (uintptr_t) ht);
		zend_opcache_static_cache_prepare_memo_mark_dependency_dirty(memo_dependency);
	}

	if (ht == NULL || !zend_opcache_static_cache_has_tracked_arrays) {
		return false;
	}

	dependency = zend_opcache_static_cache_find_tracked_array(ht);
	if (zend_opcache_static_cache_defer_dependency_publish(dependency)) {
		return false;
	}

	handle = zend_opcache_static_cache_tracked_dependency_first_handle(dependency);

	if (handle == NULL) {
		if (zend_opcache_static_cache_pending_mutation_state.dirty &&
			zend_opcache_static_cache_pending_mutation_state.handle != NULL
		) {
			zend_opcache_static_cache_reset_pending_mutation();
		}

		return false;
	}

	return zend_opcache_static_cache_begin_mutation(handle, ht);
}

static void zend_opcache_static_cache_object_mutation(zend_object *obj)
{
	zend_opcache_static_cache_tracked_dependency *dependency;
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_opcache_static_cache_prepare_memo_dependency *memo_dependency;

	if (obj != NULL && zend_opcache_static_cache_has_prepare_memo_objects && zend_opcache_static_cache_prepare_memo_objects != NULL) {
		memo_dependency = zend_hash_index_find_ptr(zend_opcache_static_cache_prepare_memo_objects, (zend_ulong) (uintptr_t) obj);
		zend_opcache_static_cache_prepare_memo_mark_dependency_dirty(memo_dependency);
	}

	if (obj == NULL || !zend_opcache_static_cache_has_tracked_objects) {
		return;
	}

	dependency = zend_opcache_static_cache_find_tracked_object(obj);
	if (zend_opcache_static_cache_defer_dependency_publish(dependency)) {
		return;
	}

	handle = zend_opcache_static_cache_tracked_dependency_first_handle(dependency);

	if (!zend_opcache_static_cache_begin_mutation(handle, NULL)) {
		return;
	}

	zend_opcache_static_cache_flush_pending();
}

static void zend_opcache_static_cache_reference_update(zend_reference *ref)
{
	zend_opcache_static_cache_static_slot_handle *handle = zend_opcache_static_cache_find_tracked_reference(ref);

	if (handle == NULL || !zend_opcache_static_cache_kind_publishes_immediately(handle->kind)) {
		return;
	}

	if (handle->snapshot_root_published && zend_opcache_static_cache_has_mutable_immediate_root(handle)) {
		return;
	}

	zend_opcache_static_cache_publish_immediate_root(handle);
}

static void zend_opcache_static_cache_init_class(zend_class_entry *ce)
{
	zend_opcache_static_cache_context *context;
	zend_opcache_static_cache_context *previous_context;
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_opcache_static_cache_kind kind;
	zend_string *property_name, *cache_key;
	zend_property_info *property_info;
	zend_long ttl;
	zval cached_value, *slot;
	bool should_track = false, fetched_cached;

	if (!zend_opcache_static_cache_attribute_classes_initialized || ce->name == NULL || CE_STATIC_MEMBERS(ce) == NULL) {
		return;
	}

	if (zend_opcache_static_cache_class_blob_enabled(ce)) {
		class_blob_handle = zend_opcache_static_cache_ensure_class_blob_handle(ce);
		if (class_blob_handle == NULL || !zend_opcache_static_cache_context_runtime(class_blob_handle->context)->available) {
			return;
		}

		EG(static_cache_class_access_active) = true;

		previous_context = zend_opcache_static_cache_activate_context(class_blob_handle->context);
		if (zend_opcache_static_cache_rlock()) {
			if (!class_blob_handle->initialized && zend_opcache_static_cache_header_is_initialized_locked()) {
				zend_opcache_static_cache_refresh_class_blob_handle_locked(class_blob_handle);
			} else if (!class_blob_handle->initialized) {
				zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
			}

			zend_opcache_static_cache_unlock();
		} else if (!class_blob_handle->initialized) {
			zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
		}

		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_install_class_blob_property_refs(class_blob_handle);

		return;
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(&ce->properties_info, property_name, property_info) {
		if (property_name == NULL) {
			continue;
		}

		fetched_cached = false;

		kind = zend_opcache_static_cache_static_property_kind_and_ttl(ce, property_info, &ttl);
		if (kind == ZEND_OPCACHE_STATIC_CACHE_NONE) {
			continue;
		}

		context = zend_opcache_static_cache_context_for_kind(kind);
		if (!zend_opcache_static_cache_context_runtime(context)->available) {
			continue;
		}

		should_track = true;

		cache_key = zend_opcache_static_cache_static_property_key(ce, property_name, kind);
		if (cache_key == NULL) {
			continue;
		}

		previous_context = zend_opcache_static_cache_activate_context(context);
		if (!zend_opcache_static_cache_rlock()) {
			zend_opcache_static_cache_restore_context(previous_context);
			zend_string_release(cache_key);
			continue;
		}

		if (!zend_opcache_static_cache_header_is_initialized_locked()) {
			zend_opcache_static_cache_unlock();
			zend_opcache_static_cache_restore_context(previous_context);
			zend_string_release(cache_key);
			continue;
		}

		slot = CE_STATIC_MEMBERS(ce) + property_info->offset;
		ZVAL_DEINDIRECT(slot);

		handle = zend_opcache_static_cache_ensure_tracked_root(slot, cache_key, context, kind, ttl, NULL);
		zend_opcache_static_cache_capture_begin_for_handle(handle);

		if (zend_opcache_static_cache_fetch_locked(cache_key, &cached_value, false, NULL, false)) {
			fetched_cached = true;
			zval_ptr_dtor(slot);
			ZVAL_COPY_VALUE(slot, &cached_value);
			zend_opcache_static_cache_set_tracked_root_original(handle);
			handle->snapshot_root_published = zend_opcache_static_cache_has_mutable_immediate_root(handle);
			handle->request_deferred_publish = zend_opcache_static_cache_kind_defers_reachable_mutation_publish(kind);
		}

		zend_opcache_static_cache_capture_active = false;

		if (!fetched_cached) {
			zend_opcache_static_cache_capture_discard();
		}

		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_restore_context(previous_context);
		zend_opcache_static_cache_register_tracked_value(slot, cache_key, kind, ttl);
		zend_string_release(cache_key);
	} ZEND_HASH_FOREACH_END();

	if (should_track) {
		zend_opcache_static_cache_track_attribute_class(ce);
	}
}

static void zend_opcache_static_cache_class_access(zend_class_entry *ce)
{
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_opcache_static_cache_context *previous_context;

	if (ce == NULL || CE_STATIC_MEMBERS(ce) == NULL) {
		return;
	}

	/* Filter inline so that classes already memoized as ignored short-circuit
	 * without paying for an extra indirect call from the VM. */
	if (!zend_opcache_static_cache_class_access_filter(ce, &class_blob_handle)) {
		return;
	}

	if (class_blob_handle == NULL && zend_opcache_static_cache_class_blob_enabled(ce)) {
		class_blob_handle = zend_opcache_static_cache_ensure_class_blob_handle(ce);
	}

	if (class_blob_handle == NULL) {
		zend_opcache_static_cache_init_class(ce);
		return;
	}

	if (!zend_opcache_static_cache_context_runtime(class_blob_handle->context)->available) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(class_blob_handle->context);

	if (zend_opcache_static_cache_rlock()) {
		if (!class_blob_handle->initialized && zend_opcache_static_cache_header_is_initialized_locked()) {
			zend_opcache_static_cache_refresh_class_blob_handle_locked(class_blob_handle);
		} else if (!class_blob_handle->initialized) {
			zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
		}

		zend_opcache_static_cache_unlock();
	} else if (!class_blob_handle->initialized) {
		zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
	}

	zend_opcache_static_cache_restore_context(previous_context);

	zend_opcache_static_cache_install_class_blob_property_refs(class_blob_handle);
}

static void zend_opcache_static_cache_class_update(zend_class_entry *ce)
{
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;

	if (ce == NULL || CE_STATIC_MEMBERS(ce) == NULL) {
		return;
	}

	class_blob_handle = zend_opcache_static_cache_find_class_blob_handle(ce);
	if (class_blob_handle == NULL) {
		zend_opcache_static_cache_publish_stable_static_properties_fast(ce);

		return;
	}

	zend_opcache_static_cache_publish_class_blob_fast(class_blob_handle);
}

static void zend_opcache_static_cache_init_function(zend_execute_data *execute_data)
{
	zend_opcache_static_cache_class_blob_handle *class_blob_handle;
	zend_opcache_static_cache_kind kind;
	zend_opcache_static_cache_context *context, *previous_context;
	zend_opcache_static_cache_static_slot_handle *handle;
	zend_op_array *op_array;
	zend_string *var_name, *cache_key;
	zend_long ttl;
	zval cached_value, *slot;
	HashTable *ht;
	bool already_initialized = false, fetched_cached;

	if (!zend_opcache_static_cache_function_statics_initialized ||
		execute_data->func->type != ZEND_USER_FUNCTION
	) {
		return;
	}

	op_array = &execute_data->func->op_array;
	/* Fast skip: if the op_array has neither function-level attributes nor a
	 * class scope carrying attributes, neither static-cache branch
	 * can fire. This avoids the per-BIND_STATIC has_attribute lookup for every
	 * ordinary `static $x` declaration. */
	if (op_array->attributes == NULL
		&& (op_array->scope == NULL || op_array->scope->attributes == NULL)
	) {
		return;
	}

	if (zend_opcache_static_cache_class_blob_applies_to_function_static(op_array)) {
		ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
		if (ht == NULL) {
			ht = zend_array_dup(op_array->static_variables);
			ZEND_MAP_PTR_SET(op_array->static_variables_ptr, ht);
		}

		class_blob_handle = zend_opcache_static_cache_ensure_class_blob_handle(op_array->scope);
		if (class_blob_handle == NULL || !zend_opcache_static_cache_context_runtime(class_blob_handle->context)->available) {
			return;
		}

		EG(static_cache_class_access_active) = true;

		previous_context = zend_opcache_static_cache_activate_context(class_blob_handle->context);
		if (zend_opcache_static_cache_rlock()) {
			if (!class_blob_handle->initialized && zend_opcache_static_cache_header_is_initialized_locked()) {
				zend_opcache_static_cache_refresh_class_blob_handle_locked(class_blob_handle);
			} else if (!class_blob_handle->initialized) {
				zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
			}

			zend_opcache_static_cache_unlock();
		} else if (!class_blob_handle->initialized) {
			zend_opcache_static_cache_build_default_class_blob_state(class_blob_handle);
		}

		zend_opcache_static_cache_restore_context(previous_context);

		if (CE_STATIC_MEMBERS(op_array->scope) != NULL) {
			zend_opcache_static_cache_install_class_blob_property_refs(class_blob_handle);
		}

		zend_opcache_static_cache_bind_class_blob_method_refs(class_blob_handle, op_array, ht);

		return;
	}

	if (!zend_opcache_static_cache_applies_to_function_static(op_array)) {
		return;
	}

	kind = zend_opcache_static_cache_op_array_kind_and_ttl(op_array, &ttl);
	context = zend_opcache_static_cache_context_for_kind(kind);
	if (!zend_opcache_static_cache_context_runtime(context)->available) {
		return;
	}

	ht = ZEND_MAP_PTR_GET(op_array->static_variables_ptr);
	if (ht == NULL) {
		ht = zend_array_dup(op_array->static_variables);
		ZEND_MAP_PTR_SET(op_array->static_variables_ptr, ht);
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(ht, var_name, slot) {
		if (var_name == NULL) {
			continue;
		}

		if (zend_hash_index_exists(&zend_opcache_static_cache_function_statics, (zend_ulong) (uintptr_t) slot)) {
			already_initialized = true;
			break;
		}
	} ZEND_HASH_FOREACH_END();

	if (already_initialized) {
		return;
	}

	previous_context = zend_opcache_static_cache_activate_context(context);

	if (!zend_opcache_static_cache_rlock()) {
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	if (!zend_opcache_static_cache_header_is_initialized_locked()) {
		zend_opcache_static_cache_unlock();
		zend_opcache_static_cache_restore_context(previous_context);

		return;
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(ht, var_name, slot) {
		if (var_name == NULL) {
			continue;
		}

		fetched_cached = false;

		cache_key = zend_opcache_static_cache_function_variable_key(op_array, var_name, kind);
		if (cache_key == NULL) {
			continue;
		}

		handle = zend_opcache_static_cache_ensure_tracked_root(slot, cache_key, context, kind, ttl, NULL);
		zend_opcache_static_cache_capture_begin_for_handle(handle);

		if (zend_opcache_static_cache_fetch_locked(cache_key, &cached_value, false, NULL, false)) {
			fetched_cached = true;
			zval_ptr_dtor(slot);
			ZVAL_REF(slot, zend_opcache_static_cache_create_reference(&cached_value));
			zend_opcache_static_cache_set_tracked_root_original(handle);
			handle->request_deferred_publish = zend_opcache_static_cache_kind_defers_reachable_mutation_publish(kind);
		}

		zend_opcache_static_cache_capture_active = false;

		if (!fetched_cached) {
			zend_opcache_static_cache_capture_discard();
		}

		zend_opcache_static_cache_track_function_static_slot(slot, cache_key, kind, ttl);
		zend_opcache_static_cache_register_tracked_value(slot, cache_key, kind, ttl);
		zend_string_release(cache_key);
	} ZEND_HASH_FOREACH_END();

	zend_opcache_static_cache_unlock();
	zend_opcache_static_cache_restore_context(previous_context);
}

void zend_opcache_static_cache_update_mutation_hook_state(void)
{
	EG(tracked_mutation_hooks_active) =
		zend_opcache_static_cache_has_tracked_arrays ||
		zend_opcache_static_cache_has_tracked_objects ||
		zend_opcache_static_cache_has_prepare_memo_arrays ||
		zend_opcache_static_cache_has_prepare_memo_objects
	;
}

bool zend_opcache_static_cache_prepare_memo_fetch(
		zval *value,
		zend_opcache_static_cache_prepared_value *prepared)
{
	zend_opcache_static_cache_prepare_memo_entry *entry;

	if (value == NULL || prepared == NULL) {
		return false;
	}

	entry = zend_opcache_static_cache_prepare_memo_find_root_entry(value);
	if (entry == NULL) {
		return false;
	}

	prepared->value_type = ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH;
	prepared->value_len = entry->value_len;
	prepared->payload_size = entry->payload_size;
	prepared->payload_used_size = entry->payload_used_size;
	prepared->payload_source = entry->payload_buffer;

	return true;
}

void zend_opcache_static_cache_prepare_memo_store(
		zval *value,
		zend_opcache_static_cache_prepared_value *prepared)
{
	zend_opcache_static_cache_prepare_memo_entry *entry;
	zend_ulong key;
	HashTable seen_arrays, seen_objects, *roots;

	if (value == NULL ||
		prepared == NULL ||
		prepared->value_type != ZEND_OPCACHE_STATIC_CACHE_VALUE_SHARED_GRAPH ||
		prepared->owned_buffer == NULL
	) {
		return;
	}

	ZVAL_DEREF(value);
	if (Z_TYPE_P(value) != IS_ARRAY && Z_TYPE_P(value) != IS_OBJECT) {
		return;
	}

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);
	if (!zend_opcache_static_cache_prepare_memo_can_track_value_recursive(value, &seen_arrays, &seen_objects)) {
		zend_hash_destroy(&seen_objects);
		zend_hash_destroy(&seen_arrays);

		return;
	}

	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);

	if (zend_opcache_static_cache_prepare_memo_entries == NULL) {
		zend_opcache_static_cache_prepare_memo_entries = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_prepare_memo_entries, 8, NULL, zend_opcache_static_cache_prepare_memo_entry_dtor, 0);
	}

	if (zend_opcache_static_cache_prepare_memo_array_roots == NULL) {
		zend_opcache_static_cache_prepare_memo_array_roots = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_prepare_memo_array_roots, 8, NULL, NULL, 0);
	}

	if (zend_opcache_static_cache_prepare_memo_object_roots == NULL) {
		zend_opcache_static_cache_prepare_memo_object_roots = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_prepare_memo_object_roots, 8, NULL, NULL, 0);
	}

	if (zend_opcache_static_cache_prepare_memo_arrays == NULL) {
		zend_opcache_static_cache_prepare_memo_arrays = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_prepare_memo_arrays, 8, NULL, zend_opcache_static_cache_prepare_memo_dependency_dtor, 0);
	}

	if (zend_opcache_static_cache_prepare_memo_objects == NULL) {
		zend_opcache_static_cache_prepare_memo_objects = emalloc(sizeof(HashTable));
		zend_hash_init(zend_opcache_static_cache_prepare_memo_objects, 8, NULL, zend_opcache_static_cache_prepare_memo_dependency_dtor, 0);
	}

	entry = emalloc(sizeof(*entry));
	ZVAL_COPY_DEREF(&entry->root_snapshot, value);
	entry->payload_buffer = prepared->owned_buffer;
	entry->payload_size = prepared->payload_size;
	entry->payload_used_size = prepared->payload_used_size;
	entry->value_len = prepared->value_len;
	entry->root_type = (uint8_t) Z_TYPE_P(value);
	entry->dirty = false;

	prepared->owned_buffer = NULL;
	prepared->payload_source = entry->payload_buffer;

	zend_hash_index_update_ptr(
		zend_opcache_static_cache_prepare_memo_entries,
		(zend_ulong) (uintptr_t) entry,
		entry
	);

	roots = zend_opcache_static_cache_prepare_memo_root_table_for_value(value);
	key = zend_opcache_static_cache_prepare_memo_root_key(value);
	zend_hash_index_update_ptr(roots, key, entry);

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);

	zend_opcache_static_cache_prepare_memo_track_value_recursive(value, entry, &seen_arrays, &seen_objects);
	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);
	zend_opcache_static_cache_update_mutation_hook_state();
}

void zend_opcache_static_cache_delete_script_keys_locked(zend_persistent_script *persistent_script)
{
	zend_class_entry *ce;

	if (persistent_script == NULL) {
		return;
	}

	ZEND_HASH_MAP_FOREACH_PTR(&persistent_script->script.class_table, ce) {
		zend_opcache_static_cache_delete_class_keys_locked(ce);
	} ZEND_HASH_FOREACH_END();
}

void zend_opcache_static_cache_register_hooks(void)
{
	zend_class_init_statics_hook = zend_opcache_static_cache_init_class;
	zend_function_init_statics_hook = zend_opcache_static_cache_init_function;
	zend_class_static_access_hook = zend_opcache_static_cache_class_access;
	zend_class_static_update_hook = zend_opcache_static_cache_class_update;
	zend_tracked_reference_update_hook = zend_opcache_static_cache_reference_update;
	zend_tracked_hash_mutation_hook = zend_opcache_static_cache_hash_mutation;
	zend_tracked_object_mutation_hook = zend_opcache_static_cache_object_mutation;
}

void zend_opcache_static_cache_unregister_hooks(void)
{
	if (zend_class_init_statics_hook == zend_opcache_static_cache_init_class) {
		zend_class_init_statics_hook = NULL;
	}

	if (zend_function_init_statics_hook == zend_opcache_static_cache_init_function) {
		zend_function_init_statics_hook = NULL;
	}

	if (zend_class_static_access_hook == zend_opcache_static_cache_class_access) {
		zend_class_static_access_hook = NULL;
	}

	if (zend_class_static_update_hook == zend_opcache_static_cache_class_update) {
		zend_class_static_update_hook = NULL;
	}

	if (zend_tracked_reference_update_hook == zend_opcache_static_cache_reference_update) {
		zend_tracked_reference_update_hook = NULL;
	}

	if (zend_tracked_hash_mutation_hook == zend_opcache_static_cache_hash_mutation) {
		zend_tracked_hash_mutation_hook = NULL;
	}

	if (zend_tracked_object_mutation_hook == zend_opcache_static_cache_object_mutation) {
		zend_tracked_object_mutation_hook = NULL;
	}
}

void zend_opcache_static_cache_request_init(void)
{
	zend_hash_init(&zend_opcache_static_cache_attribute_classes, 8, NULL, NULL, 0);
	zend_opcache_static_cache_attribute_classes_initialized = true;
	zend_hash_init(&zend_opcache_static_cache_ignored_classes, 8, NULL, NULL, 0);
	zend_opcache_static_cache_ignored_classes_initialized = true;
	zend_hash_init(&zend_opcache_static_cache_function_statics, 8, NULL, zend_opcache_static_cache_function_static_entry_dtor, 0);
	zend_opcache_static_cache_function_statics_initialized = true;
	zend_hash_init(&zend_opcache_static_cache_class_blob_handles, 8, NULL, zend_opcache_static_cache_class_blob_handle_dtor, 0);
	zend_opcache_static_cache_class_blob_handles_initialized = true;
	zend_opcache_static_cache_init_tracking();
}

void zend_opcache_static_cache_capture_begin_for_handle(zend_opcache_static_cache_static_slot_handle *handle)
{
	zend_opcache_static_cache_capture_clear();
	if (!zend_opcache_static_cache_tracks_reachable_mutations(handle)) {
		return;
	}

	zend_opcache_static_cache_capture_handle = handle;
	zend_opcache_static_cache_capture_active = true;
}

void zend_opcache_static_cache_capture_discard(void)
{
	zend_opcache_static_cache_capture_active = false;
	zend_opcache_static_cache_capture_clear();
}

void zend_opcache_static_cache_capture_decoded_reachable_value(zval *value)
{
	HashTable seen_arrays, seen_objects;

	if (!zend_opcache_static_cache_capture_active || value == NULL) {
		return;
	}

	zend_hash_init(&seen_arrays, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objects, 8, NULL, NULL, 0);

	zend_opcache_static_cache_capture_decoded_reachable_value_ex(value, &seen_arrays, &seen_objects);

	zend_hash_destroy(&seen_objects);
	zend_hash_destroy(&seen_arrays);
}

void zend_opcache_static_cache_capture_decoded_value(zval *value)
{
	zend_ulong key;
	HashTable *table;

	if (!zend_opcache_static_cache_capture_active || value == NULL) {
		return;
	}

	ZVAL_DEREF(value);
	switch (Z_TYPE_P(value)) {
		case IS_ARRAY:
			table = zend_opcache_static_cache_capture_ensure_table(&zend_opcache_static_cache_capture_arrays);
			key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(value);

			if (zend_opcache_static_cache_capture_handle != NULL) {
				zend_hash_index_update_ptr(table, key, zend_opcache_static_cache_capture_handle);
			} else {
				zend_hash_index_add_empty_element(table, key);
			}

			zend_opcache_static_cache_capture_available = true;

			return;
		case IS_OBJECT:
			table = zend_opcache_static_cache_capture_ensure_table(&zend_opcache_static_cache_capture_objects);
			key = (zend_ulong) (uintptr_t) Z_OBJ_P(value);

			if (zend_opcache_static_cache_capture_handle != NULL) {
				zend_hash_index_update_ptr(table, key, zend_opcache_static_cache_capture_handle);
			} else {
				zend_hash_index_add_empty_element(table, key);
			}

			zend_opcache_static_cache_capture_available = true;

			return;
		default:
			return;
	}
}

void zend_opcache_static_cache_request_shutdown(void)
{
	if (zend_opcache_static_cache_attribute_classes_initialized ||
		zend_opcache_static_cache_function_statics_initialized ||
		zend_opcache_static_cache_class_blob_handles_initialized
	) {
		zend_opcache_static_cache_flush_pending();

		zend_opcache_static_cache_publish_context(zend_opcache_static_cache_active_stable_context());
		zend_opcache_static_cache_publish_context(zend_opcache_static_cache_active_volatile_context());

		if (zend_opcache_static_cache_attribute_classes_initialized) {
			zend_hash_destroy(&zend_opcache_static_cache_attribute_classes);
			zend_opcache_static_cache_attribute_classes_initialized = false;
		}

		if (zend_opcache_static_cache_ignored_classes_initialized) {
			zend_hash_destroy(&zend_opcache_static_cache_ignored_classes);
			zend_opcache_static_cache_ignored_classes_initialized = false;
		}

		if (zend_opcache_static_cache_function_statics_initialized) {
			zend_hash_destroy(&zend_opcache_static_cache_function_statics);
			zend_opcache_static_cache_function_statics_initialized = false;
		}

		if (zend_opcache_static_cache_class_blob_handles_initialized) {
			zend_hash_destroy(&zend_opcache_static_cache_class_blob_handles);
			zend_opcache_static_cache_class_blob_handles_initialized = false;
		}
	}

	zend_opcache_static_cache_destroy_tracking();
}
