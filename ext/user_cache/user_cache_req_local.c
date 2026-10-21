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

#include "user_cache_entries.h"

#include "Zend/zend_objects.h"

#define UCACHE_REQ_LOCAL_NO_DEEP_CLONE		((void *) 1)
#define UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE	((void *) 2)

typedef struct {
	HashTable arrs;
	HashTable objs;
	HashTable refs;
	HashTable *clone_verdicts;
	bool *proto_rejected;
	bool track_identity;
} ucache_req_local_clone_ctx;

static bool ucache_clone_req_local_val(
		ucache_req_local_clone_ctx *ctx,
		zval *dst,
		zval *src);

static bool ucache_collect_req_local_clone_verdicts_impl(
		zval *val,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		HashTable *verdicts,
		bool record_arr_result);

static zend_always_inline ucache_req_local_slot *ucache_alloc_req_local_slot(
		uint64_t gen,
		bool no_aliases,
		bool has_val,
		bool needs_deep_clone)
{
	ucache_req_local_slot *slot;

	slot = emalloc(needs_deep_clone
		? sizeof(ucache_req_local_slot_with_verdicts)
		: sizeof(ucache_req_local_slot)
	);

	slot->gen = gen;
	slot->ctx = ucache_active_ctx();
	slot->needs_deep_clone = needs_deep_clone;
	slot->has_clone_verdicts = false;
	slot->no_aliases = no_aliases;
	slot->has_val = has_val;
	slot->proto_rejected = false;
	slot->released_while_cloning = false;
	slot->clone_depth = 0;

	ZVAL_UNDEF(&slot->val);

	return slot;
}

static zend_always_inline bool ucache_obj_has_deprecated_dynamic_props(const zend_object *obj)
{
	zval *prop;

	if (obj->properties == NULL || (obj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
		return false;
	}

	ZEND_HASH_MAP_FOREACH_VAL(obj->properties, prop) {
		if (Z_TYPE_P(prop) != IS_INDIRECT) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

static zend_always_inline bool ucache_proto_rejects_obj(const zend_object *obj)
{
	if (obj->ce->destructor != NULL || ucache_obj_has_deprecated_dynamic_props(obj)) {
		return true;
	}

	return obj->handlers == zend_get_std_object_handlers() &&
		(
			obj->ce->__unserialize != NULL ||
			zend_hash_find_known_hash(
				&obj->ce->function_table,
				ZSTR_KNOWN(ZEND_STR_WAKEUP)
			) != NULL
		)
	;
}

static void ucache_req_local_slot_dtor(zval *slot_zv)
{
	ucache_req_local_slot_free(Z_PTR_P(slot_zv));
}

static bool ucache_val_needs_req_local_deep_clone_impl(
		zval *val,
		HashTable *seen_arrs)
{
	zval *elem;

	if (ucache_stack_exhausted()) {
		return true;
	}

	switch (Z_TYPE_P(val)) {
		case IS_REFERENCE:
		case IS_OBJECT:
			return true;
		case IS_ARRAY:
			if (GC_FLAGS(Z_ARRVAL_P(val)) & IS_ARRAY_IMMUTABLE) {
				return false;
			}

			if (!ucache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(val))) {
				return false;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), elem) {
				if (ucache_val_needs_req_local_deep_clone_impl(elem, seen_arrs)) {
					return true;
				}
			} ZEND_HASH_FOREACH_END();

			return false;
		default:
			return false;
	}
}

static bool ucache_val_needs_req_local_deep_clone(zval *val)
{
	HashTable seen_arrs;
	bool result;

	ZEND_ASSERT(Z_TYPE_P(val) == IS_ARRAY);

	if (GC_FLAGS(Z_ARRVAL_P(val)) & IS_ARRAY_IMMUTABLE) {
		return false;
	}

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);

	result = ucache_val_needs_req_local_deep_clone_impl(val, &seen_arrs);

	zend_hash_destroy(&seen_arrs);

	return result;
}

static void ucache_collect_req_local_obj_clone_verdict(
		zend_object *obj,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		HashTable *verdicts)
{
	zend_ulong obj_key;
	zval *prop, *src, *end;
	bool members_need_clone = false;

	if (ucache_stack_exhausted()) {
		UC_G(req_local_slot_may_cycle) = true;

		return;
	}

	if (obj == NULL) {
		return;
	}

	obj_key = (zend_ulong) (uintptr_t) obj;

	if (zend_hash_index_exists(verdicts, obj_key)) {
		return;
	}

	if (!ucache_seen_test_and_add(seen_objs, obj)) {
		UC_G(req_local_slot_may_cycle) = true;

		return;
	}

	if (obj->ce->default_properties_count) {
		src = obj->properties_table;
		end = src + obj->ce->default_properties_count;

		do {
			if (ucache_collect_req_local_clone_verdicts_impl(
					src,
					seen_arrs,
					seen_objs,
					verdicts,
					true
				)
			) {
				members_need_clone = true;
			}

			src++;
		} while (src != end);
	}

	if (obj->properties != NULL && zend_hash_num_elements(obj->properties) != 0) {
		members_need_clone = true;

		ZEND_HASH_MAP_FOREACH_VAL(obj->properties, prop) {
			if (Z_TYPE_P(prop) != IS_INDIRECT) {
				ucache_collect_req_local_clone_verdicts_impl(
					prop,
					seen_arrs,
					seen_objs,
					verdicts,
					true
				);
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_hash_index_update_ptr(
		verdicts,
		obj_key,
		members_need_clone
			? UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE
			: UCACHE_REQ_LOCAL_NO_DEEP_CLONE
	);
}

static bool ucache_collect_req_local_clone_verdicts_impl(
		zval *val,
		HashTable *seen_arrs,
		HashTable *seen_objs,
		HashTable *verdicts,
		bool record_arr_result)
{
	zend_ulong arr_key;
	zval *elem;
	bool needs_deep_clone = false;
	void *flag;

	if (ucache_stack_exhausted()) {
		UC_G(req_local_slot_may_cycle) = true;

		return true;
	}

	switch (Z_TYPE_P(val)) {
		case IS_REFERENCE:
			ucache_collect_req_local_clone_verdicts_impl(
				&Z_REF_P(val)->val,
				seen_arrs,
				seen_objs,
				verdicts,
				true
			);

			return true;
		case IS_OBJECT:
			ucache_collect_req_local_obj_clone_verdict(
				Z_OBJ_P(val),
				seen_arrs,
				seen_objs,
				verdicts
			);

			return true;
		case IS_ARRAY:
			if (GC_FLAGS(Z_ARRVAL_P(val)) & IS_ARRAY_IMMUTABLE) {
				return false;
			}

			arr_key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(val);
			flag = zend_hash_index_find_ptr(verdicts, arr_key);
			if (flag != NULL) {
				return flag == UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE;
			}

			if (!ucache_seen_test_and_add(seen_arrs, Z_ARRVAL_P(val))) {
				UC_G(req_local_slot_may_cycle) = true;

				return true;
			}

			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(val), elem) {
				if (ucache_collect_req_local_clone_verdicts_impl(
						elem,
						seen_arrs,
						seen_objs,
						verdicts,
						false
					)
				) {
					needs_deep_clone = true;
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_index_del(seen_arrs, arr_key);

			if (needs_deep_clone || record_arr_result || GC_REFCOUNT(Z_ARRVAL_P(val)) > 1) {
				zend_hash_index_update_ptr(
					verdicts,
					arr_key,
					needs_deep_clone
						? UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE
						: UCACHE_REQ_LOCAL_NO_DEEP_CLONE
				);
			}

			return needs_deep_clone;
		default:
			return false;
	}
}

static bool ucache_collect_req_local_clone_verdicts(
		zval *val,
		HashTable *verdicts)
{
	HashTable seen_arrs, seen_objs;
	bool result;

	zend_hash_init(&seen_arrs, 8, NULL, NULL, 0);
	zend_hash_init(&seen_objs, 8, NULL, NULL, 0);

	result = ucache_collect_req_local_clone_verdicts_impl(
		val,
		&seen_arrs,
		&seen_objs,
		verdicts,
		true
	);

	zend_hash_destroy(&seen_objs);
	zend_hash_destroy(&seen_arrs);

	return result;
}

static void ucache_req_local_clone_arr_dtor(zval *zv)
{
	zend_array *arr = Z_PTR_P(zv);

	zend_array_release(arr);
}

static void ucache_req_local_clone_linked_dtor(zval *zv)
{
	zend_refcounted *counted = Z_PTR_P(zv);

	if (GC_DELREF(counted) == 0) {
		rc_dtor_func(counted);
	}
}

static void ucache_req_local_clone_ctx_init(
		ucache_req_local_clone_ctx *ctx,
		HashTable *verdicts,
		bool track_identity,
		bool *proto_rejected)
{
	if (track_identity) {
		zend_hash_init(&ctx->arrs, 8, NULL, ucache_req_local_clone_arr_dtor, 0);
		zend_hash_init(&ctx->objs, 8, NULL, ucache_obj_table_dtor, 0);
		zend_hash_init(&ctx->refs, 8, NULL, ucache_ref_table_dtor, 0);
	}

	ctx->clone_verdicts = verdicts;
	ctx->proto_rejected = proto_rejected;
	ctx->track_identity = track_identity;
}

static void ucache_req_local_clone_ctx_destroy(
		ucache_req_local_clone_ctx *ctx,
		bool linked)
{
	if (ctx->track_identity) {
		if (linked) {
			ctx->refs.pDestructor = ucache_req_local_clone_linked_dtor;
			ctx->objs.pDestructor = ucache_req_local_clone_linked_dtor;
		}

		zend_hash_destroy(&ctx->refs);
		zend_hash_destroy(&ctx->objs);
		zend_hash_destroy(&ctx->arrs);
	}
}

static bool ucache_val_needs_req_local_deep_clone_cached(
		ucache_req_local_clone_ctx *ctx,
		zval *val)
{
	void *flag;

	switch (Z_TYPE_P(val)) {
		case IS_REFERENCE:
		case IS_OBJECT:
			return true;
		case IS_ARRAY:
			break;
		default:
			return false;
	}

	if (ctx->clone_verdicts != NULL) {
		flag = zend_hash_index_find_ptr(ctx->clone_verdicts, (zend_ulong) (uintptr_t) Z_ARRVAL_P(val));
		if (flag != NULL) {
			return flag == UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE;
		}
	}

	return ucache_val_needs_req_local_deep_clone(val);
}

static bool ucache_clone_req_local_arr(
		ucache_req_local_clone_ctx *ctx,
		zval *dst,
		zval *src,
		bool known_needs_deep_clone)
{
	zend_ulong key = (zend_ulong) (uintptr_t) Z_ARRVAL_P(src);
	zend_array *arr;
	zval *elem, cloned_elem;
	void *flag;

	if (!known_needs_deep_clone) {
		if (GC_FLAGS(Z_ARRVAL_P(src)) & IS_ARRAY_IMMUTABLE) {
			ZVAL_COPY(dst, src);

			return true;
		}

		if (ctx->clone_verdicts != NULL) {
			flag = zend_hash_index_find_ptr(ctx->clone_verdicts, key);
			if (flag == UCACHE_REQ_LOCAL_NO_DEEP_CLONE) {
				ZVAL_COPY(dst, src);

				return true;
			} else if (flag == UCACHE_REQ_LOCAL_NEEDS_DEEP_CLONE) {
				known_needs_deep_clone = true;
			}
		}

		if (!known_needs_deep_clone && !ucache_val_needs_req_local_deep_clone(src)) {
			ZVAL_COPY(dst, src);

			return true;
		}
	}

	if (ctx->track_identity) {
		arr = zend_hash_index_find_ptr(&ctx->arrs, key);
		if (arr != NULL) {
			GC_ADDREF(arr);

			ZVAL_ARR(dst, arr);

			return true;
		}
	}

	arr = zend_array_dup(Z_ARRVAL_P(src));
	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->arrs, key, arr);
	}

	ZEND_HASH_FOREACH_VAL(arr, elem) {
		if (Z_TYPE_P(elem) == IS_INDIRECT ||
			!ucache_val_needs_req_local_deep_clone_cached(ctx, elem)
		) {
			continue;
		}

		if (!ucache_clone_req_local_val(ctx, &cloned_elem, elem)) {
			if (!ctx->track_identity) {
				zend_array_release(arr);
			}

			ZVAL_UNDEF(dst);

			return false;
		}

		zval_ptr_dtor_nogc(elem);

		ZVAL_COPY_VALUE(elem, &cloned_elem);
	} ZEND_HASH_FOREACH_END();

	if (ctx->track_identity) {
		GC_ADDREF(arr);
	}

	ZVAL_ARR(dst, arr);

	return true;
}

static bool ucache_clone_req_local_ref(
		ucache_req_local_clone_ctx *ctx,
		zval *dst,
		zend_reference *src_ref)
{
	zend_ulong key = (zend_ulong) (uintptr_t) src_ref;
	zend_reference *new_ref;
	zval inner;

	if (ctx->track_identity) {
		new_ref = zend_hash_index_find_ptr(&ctx->refs, key);
		if (new_ref != NULL) {
			GC_ADDREF(new_ref);

			ZVAL_REF(dst, new_ref);

			UC_G(req_local_slot_may_cycle) = true;

			return true;
		}
	}

	ZVAL_NEW_EMPTY_REF(dst);
	new_ref = Z_REF_P(dst);

	ZVAL_UNDEF(&new_ref->val);

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->refs, key, new_ref);
	}

	if (!ucache_clone_req_local_val(ctx, &inner, &src_ref->val)) {
		if (!ctx->track_identity) {
			zval_ptr_dtor(dst);
		}

		ZVAL_UNDEF(dst);

		return false;
	}

	ZVAL_COPY_VALUE(&new_ref->val, &inner);

	if (ctx->track_identity) {
		GC_ADDREF(new_ref);

		ZVAL_REF(dst, new_ref);
	}

	return true;
}

static bool ucache_clone_req_local_obj_members(
		ucache_req_local_clone_ctx *ctx,
		zend_object *old_obj,
		zend_object *new_obj)
{
	zend_ulong num_key;
	zend_string *key;
	zend_property_info *prop_info;
	zval *src, *dst, *end, *prop, new_prop;
	uint32_t prop_idx = 0;

	if (old_obj->ce->default_properties_count) {
		src = old_obj->properties_table;
		dst = new_obj->properties_table;
		end = src + old_obj->ce->default_properties_count;

		do {
			if (!ucache_clone_req_local_val(ctx, &new_prop, src)) {
				return false;
			}

			zval_ptr_dtor(dst);

			ZVAL_COPY_VALUE(dst, &new_prop);
			Z_PROP_FLAG_P(dst) = Z_PROP_FLAG_P(src);

			if (Z_ISREF_P(dst) && new_obj->ce->properties_info_table != NULL) {
				prop_info = new_obj->ce->properties_info_table[prop_idx];
				if (prop_info != NULL &&
					prop_info != ZEND_WRONG_PROPERTY_INFO &&
					ZEND_TYPE_IS_SET(prop_info->type)
				) {
					ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(dst), prop_info);
				}
			}

			src++;
			dst++;
			prop_idx++;
		} while (src != end);
	}

	if (old_obj->properties != NULL && zend_hash_num_elements(old_obj->properties) != 0) {
		if (new_obj->properties != NULL) {
			zend_hash_clean(new_obj->properties);
			zend_hash_extend(
				new_obj->properties,
				zend_hash_num_elements(old_obj->properties),
				0
			);
		} else {
			new_obj->properties = zend_new_array(zend_hash_num_elements(old_obj->properties));

			zend_hash_real_init_mixed(new_obj->properties);
		}

		HT_FLAGS(new_obj->properties) |=
			HT_FLAGS(old_obj->properties) & HASH_FLAG_HAS_EMPTY_IND
		;

		ZEND_HASH_MAP_FOREACH_KEY_VAL(old_obj->properties, num_key, key, prop) {
			if (Z_TYPE_P(prop) == IS_INDIRECT) {
				ZVAL_INDIRECT(
					&new_prop,
					new_obj->properties_table + (Z_INDIRECT_P(prop) - old_obj->properties_table)
				);
			} else if (!ucache_clone_req_local_val(ctx, &new_prop, prop)) {
				return false;
			}

			if (key != NULL) {
				_zend_hash_append(new_obj->properties, key, &new_prop);
			} else {
				zend_hash_index_add_new(new_obj->properties, num_key, &new_prop);
			}
		} ZEND_HASH_FOREACH_END();
	}

	return true;
}

static bool ucache_clone_req_local_std_obj(
		ucache_req_local_clone_ctx *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	zend_object *new_obj;
	zval *src, *dst, *end;
	void *member_flag = NULL;

	new_obj = zend_objects_new(old_obj->ce);

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(
			&ctx->objs,
			(zend_ulong) (uintptr_t) old_obj,
			new_obj
		);
	}

	if (ctx->clone_verdicts != NULL) {
		member_flag = zend_hash_index_find_ptr(
			ctx->clone_verdicts,
			(zend_ulong) (uintptr_t) old_obj
		);
	}

	if (member_flag == UCACHE_REQ_LOCAL_NO_DEEP_CLONE &&
		old_obj->properties == NULL
	) {
		if (old_obj->ce->default_properties_count) {
			src = old_obj->properties_table;
			dst = new_obj->properties_table;
			end = src + old_obj->ce->default_properties_count;

			memcpy(dst, src, sizeof(zval) * old_obj->ce->default_properties_count);

			do {
				if (Z_REFCOUNTED_P(src)) {
					Z_ADDREF_P(src);
				}

				src++;
			} while (src != end);
		}
	} else {
		if (new_obj->ce->default_properties_count) {
			dst = new_obj->properties_table;
			end = dst + new_obj->ce->default_properties_count;

			do {
				ZVAL_UNDEF(dst);

				dst++;
			} while (dst != end);
		}

		if (!ucache_clone_req_local_obj_members(ctx, old_obj, new_obj)) {
			if (!ctx->track_identity) {
				OBJ_RELEASE(new_obj);
			}

			return false;
		}
	}

	if (ctx->track_identity) {
		GC_ADDREF(new_obj);
	}

	*new_obj_ptr = new_obj;

	return true;
}

static bool ucache_clone_req_local_val_cb(
		void *ctx,
		zval *dst,
		zval *src)
{
	return ucache_clone_req_local_val(
		(ucache_req_local_clone_ctx *) ctx,
		dst,
		src
	);
}

static bool ucache_clone_req_local_safe_direct_obj(
		ucache_req_local_clone_ctx *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	php_ucache_safe_direct_copy_func_t copy_func;
	zend_ulong key;
	zend_class_entry *ce;
	zend_object *new_obj;
	zval new_zv;

	ce = old_obj->ce;

	copy_func = ucache_safe_direct_copy_func(ce);
	if (copy_func == NULL) {
		if (ctx->proto_rejected != NULL) {
			*ctx->proto_rejected = true;
		}

		return false;
	}

	ZVAL_UNDEF(&new_zv);

	if (object_init_ex(&new_zv, ce) != SUCCESS) {
		return false;
	}

	new_obj = Z_OBJ(new_zv);
	key = (zend_ulong) (uintptr_t) old_obj;

	if (ctx->track_identity) {
		zend_hash_index_update_ptr(&ctx->objs, key, new_obj);
	}

	if (!copy_func(
			ctx,
			new_obj,
			old_obj,
			ucache_clone_req_local_val_cb
		)
	) {
		if (ctx->proto_rejected != NULL) {
			*ctx->proto_rejected = true;
		}

		if (!ctx->track_identity) {
			OBJ_RELEASE(new_obj);
		}

		return false;
	}

	if (!ucache_clone_req_local_obj_members(ctx, old_obj, new_obj)) {
		if (!ctx->track_identity) {
			OBJ_RELEASE(new_obj);
		}

		return false;
	}

	if (ctx->track_identity) {
		GC_ADDREF(new_obj);
	}

	*new_obj_ptr = new_obj;

	return true;
}

static bool ucache_clone_req_local_obj(
		ucache_req_local_clone_ctx *ctx,
		zend_object *old_obj,
		zend_object **new_obj_ptr)
{
	zend_ulong key;
	zend_object *new_obj;

	if (old_obj == NULL || zend_object_is_lazy(old_obj)) {
		return false;
	}

	if (old_obj->ce->ce_flags & ZEND_ACC_ENUM) {
		GC_ADDREF(old_obj);

		*new_obj_ptr = old_obj;

		return true;
	}

	if (ctx->track_identity) {
		key = (zend_ulong) (uintptr_t) old_obj;
		new_obj = zend_hash_index_find_ptr(&ctx->objs, key);

		if (new_obj != NULL) {
			GC_ADDREF(new_obj);

			*new_obj_ptr = new_obj;

			UC_G(req_local_slot_may_cycle) = true;

			return true;
		}
	}

	if (ctx->proto_rejected != NULL && ucache_proto_rejects_obj(old_obj)) {
		*ctx->proto_rejected = true;

		return false;
	}

	if (old_obj->handlers == zend_get_std_object_handlers()) {
		return ucache_clone_req_local_std_obj(ctx, old_obj, new_obj_ptr);
	}

	return ucache_clone_req_local_safe_direct_obj(ctx, old_obj, new_obj_ptr);
}

static bool ucache_clone_req_local_val(
		ucache_req_local_clone_ctx *ctx,
		zval *dst,
		zval *src)
{
	zend_object *obj;

	if (ucache_stack_exhausted()) {
		ZVAL_UNDEF(dst);

		return false;
	}

	switch (Z_TYPE_P(src)) {
		case IS_REFERENCE:
			return ucache_clone_req_local_ref(ctx, dst, Z_REF_P(src));
		case IS_ARRAY:
			return ucache_clone_req_local_arr(ctx, dst, src, false);
		case IS_OBJECT:
			if (!ucache_clone_req_local_obj(ctx, Z_OBJ_P(src), &obj)) {
				return false;
			}

			ZVAL_OBJ(dst, obj);

			return true;
		default:
			ZVAL_COPY(dst, src);

			return true;
	}
}

static HashTable *ucache_req_local_slots(void)
{
	HashTable **slots_ptr = &UC_G(req_local_slot_table);

	ucache_req_local_slots_check_fork();

	if (*slots_ptr == NULL) {
		ALLOC_HASHTABLE(*slots_ptr);

		zend_hash_init(*slots_ptr, 0, NULL, ucache_req_local_slot_dtor, 0);

		UC_G(req_local_slot_owner_pid) = ucache_cached_pid();
	}

	return *slots_ptr;
}

static void ucache_replace_req_local_slot(
		zend_string *key,
		ucache_req_local_slot *slot)
{
	ucache_req_local_slot *old_slot;
	HashTable *slots = ucache_req_local_slots();
	zval *entry, old_zv;

	entry = zend_hash_lookup(slots, key);
	if (Z_TYPE_P(entry) == IS_NULL) {
		ZVAL_PTR(entry, slot);

		return;
	}

	old_slot = Z_PTR_P(entry);

	ZVAL_PTR(entry, slot);

	ZVAL_PTR(&old_zv, old_slot);

	ucache_req_local_slot_dtor(&old_zv);
}

static void ucache_release_req_local_slot_keys(
		HashTable **slots_ptr,
		HashTable *slots,
		zend_string **keys,
		uint32_t count)
{
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (*slots_ptr == slots) {
			zend_hash_del(slots, keys[i]);
		}

		zend_string_release(keys[i]);
	}

	efree(keys);

	if (*slots_ptr == slots && zend_hash_num_elements(slots) == 0) {
		ucache_release_req_local_slot_table(slots_ptr);
	}
}

static bool ucache_req_local_slots_can_make_room(size_t bytes)
{
	size_t budget = ucache_req_cache_budget(), record_bytes;

	ZEND_ASSERT(UC_G(record_val_bytes) >= UC_G(req_local_slot_bytes));

	record_bytes = UC_G(record_val_bytes) - UC_G(req_local_slot_bytes);

	return record_bytes <= budget && bytes <= budget - record_bytes;
}

static void ucache_reject_req_local_proto(zend_string *key, uint64_t gen)
{
	ucache_req_local_slot *slot = ucache_find_req_local_slot(key, gen);

	if (slot != NULL && !slot->has_val) {
		slot->proto_rejected = true;
	}
}

static zend_never_inline void ucache_release_req_local_slot_impl(zend_string *key)
{
	HashTable *slots = UC_G(req_local_slot_table);
	zval *entry, detached_slot;

	if ((entry = zend_hash_find(slots, key)) == NULL) {
		return;
	}

	ZVAL_COPY_VALUE(&detached_slot, entry);
	ZVAL_PTR(entry, NULL);

	zend_hash_del(slots, key);

	if (zend_hash_num_elements(slots) == 0) {
		ucache_release_req_local_slot_table(&UC_G(req_local_slot_table));
	}

	ucache_req_local_slot_dtor(&detached_slot);
}

void ucache_req_local_slot_free(ucache_req_local_slot *slot)
{
	if (slot == NULL) {
		return;
	}

	if (slot->clone_depth != 0) {
		slot->released_while_cloning = true;

		return;
	}

	if (slot->has_clone_verdicts) {
		zend_hash_destroy(ucache_req_local_slot_verdicts(slot));
	}

	if (slot->has_val) {
		UC_G(record_val_bytes) -= Z_EXTRA(slot->val);
		UC_G(req_local_slot_bytes) -= Z_EXTRA(slot->val);
	}

	if (!Z_ISUNDEF(slot->val)) {
		zval_ptr_dtor(&slot->val);
	}

	efree(slot);
}

bool ucache_clone_req_local_slot_val_known(
		zval *dst,
		zval *src,
		HashTable *verdicts,
		bool no_aliases,
		bool *proto_rejected)
{
	ucache_req_local_clone_ctx ctx;
	zend_array *arr;
	bool result;

	ucache_req_local_clone_ctx_init(&ctx, verdicts, !no_aliases, proto_rejected);

	if (Z_TYPE_P(src) == IS_ARRAY) {
		result = ucache_clone_req_local_arr(&ctx, dst, src, true);
	} else {
		result = ucache_clone_req_local_val(&ctx, dst, src);
	}

	if (!result && proto_rejected != NULL && ctx.track_identity) {
		ZEND_HASH_FOREACH_PTR(&ctx.arrs, arr) {
			HT_ALLOW_COW_VIOLATION(arr);

			zend_hash_clean(arr);
		} ZEND_HASH_FOREACH_END();
	}

	ucache_req_local_clone_ctx_destroy(&ctx, result);

	return result;
}

zend_never_inline void ucache_req_local_slots_drop_oldest_half(void)
{
	zend_string *key, **keys;
	HashTable *slots, **slots_ptr = &UC_G(req_local_slot_table);
	uint32_t drop, count = 0;

	slots = *slots_ptr;
	if (slots == NULL) {
		return;
	}

	drop = zend_hash_num_elements(slots) - zend_hash_num_elements(slots) / 2;
	keys = safe_emalloc(drop, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY(slots, key) {
		if (count == drop) {
			break;
		}

		keys[count++] = zend_string_copy(key);
	} ZEND_HASH_FOREACH_END();

	ucache_release_req_local_slot_keys(slots_ptr, slots, keys, count);
}

zend_never_inline void ucache_store_req_local_slot(
		zend_string *key,
		uint64_t gen,
		zval *val,
		bool no_aliases,
		uint32_t charged_bytes)
{
	ucache_req_local_slot *slot;
	HashTable verdicts, *slot_verdicts;
	zval slot_zv;
	bool needs_deep_clone = false, has_verdicts = false, proto_rejected = false;

	if (!ucache_record_val_fits(charged_bytes)) {
		if (UC_G(req_local_slot_bytes) < ucache_req_cache_budget() / 2) {
			return;
		}

		if (!ucache_req_local_slots_can_make_room(charged_bytes)) {
			ucache_reject_req_local_proto(key, gen);

			return;
		}

		ucache_req_local_slots_drop_oldest_half();

		if (!ucache_record_val_fits(charged_bytes)) {
			return;
		}
	}

	ZVAL_DEREF(val);

	if (Z_TYPE_P(val) == IS_ARRAY || Z_TYPE_P(val) == IS_OBJECT) {
		zend_hash_init(&verdicts, 8, NULL, NULL, 0);

		has_verdicts = true;
		needs_deep_clone = ucache_collect_req_local_clone_verdicts(
			val,
			&verdicts
		);
	}

	slot = ucache_alloc_req_local_slot(gen, no_aliases, true, needs_deep_clone);

	if (!ucache_clone_req_local_slot_val_known(
			&slot->val,
			val,
			has_verdicts ? &verdicts : NULL,
			no_aliases,
			&proto_rejected
		)
	) {
		if (has_verdicts) {
			zend_hash_destroy(&verdicts);
		}

		slot->has_val = false;

		ZVAL_PTR(&slot_zv, slot);

		ucache_req_local_slot_dtor(&slot_zv);

		if (proto_rejected) {
			ucache_reject_req_local_proto(key, gen);
		}

		return;
	}

	if (has_verdicts) {
		zend_hash_destroy(&verdicts);
	}

	if (needs_deep_clone) {
		slot_verdicts = ucache_req_local_slot_verdicts(slot);

		zend_hash_init(slot_verdicts, 8, NULL, NULL, 0);

		ucache_collect_req_local_clone_verdicts(&slot->val, slot_verdicts);

		slot->has_clone_verdicts = true;
	}

	if (!ucache_record_val_fits(charged_bytes)) {
		slot->has_val = false;

		ucache_req_local_slot_free(slot);

		return;
	}

	Z_EXTRA(slot->val) = charged_bytes;

	UC_G(record_val_bytes) += charged_bytes;
	UC_G(req_local_slot_bytes) += charged_bytes;

	ucache_replace_req_local_slot(key, slot);
}

void ucache_mark_req_local_slot(zend_string *key, uint64_t gen)
{
	ucache_req_local_slot *slot = ucache_alloc_req_local_slot(gen, true, false, false);

	ucache_replace_req_local_slot(key, slot);
}

void ucache_obj_table_dtor(zval *zv)
{
	zend_object *obj = Z_PTR_P(zv);

	OBJ_RELEASE(obj);
}

void ucache_ref_table_dtor(zval *zv)
{
	zval ref_zv;

	ZVAL_REF(&ref_zv, (zend_reference *) Z_PTR_P(zv));

	zval_ptr_dtor(&ref_zv);
}

void ucache_release_req_local_slots(void)
{
	ucache_release_req_local_slot_table(&UC_G(req_local_slot_table));
}

void ucache_release_req_local_slot(zend_string *key)
{
	if (UC_G(req_local_slot_table) != NULL) {
		ucache_release_req_local_slot_impl(key);
	}
}

void ucache_release_active_req_local_slots_by_prefix(zend_string *prefix)
{
	zend_string *key, **keys;
	HashTable *slots, **slots_ptr = &UC_G(req_local_slot_table);
	uint32_t slot_count, count = 0;

	if (*slots_ptr == NULL) {
		return;
	}

	slot_count = zend_hash_num_elements(*slots_ptr);
	if (slot_count == 0) {
		ucache_release_req_local_slot_table(slots_ptr);

		return;
	}

	slots = *slots_ptr;
	keys = safe_emalloc(slot_count, sizeof(zend_string *), 0);
	ZEND_HASH_FOREACH_STR_KEY(slots, key) {
		if (key != NULL &&
			ZSTR_LEN(key) >= ZSTR_LEN(prefix) &&
			memcmp(
				ZSTR_VAL(key),
				ZSTR_VAL(prefix),
				ZSTR_LEN(prefix)
			) == 0
		) {
			keys[count++] = zend_string_copy(key);
		}
	} ZEND_HASH_FOREACH_END();

	ucache_release_req_local_slot_keys(slots_ptr, slots, keys, count);
}
