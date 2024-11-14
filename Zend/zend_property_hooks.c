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
   | Authors: Ilija Tovilo <ilutov@php.net>                               |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_hash.h"
#include "zend_lazy_objects.h"
#include "zend_property_hooks.h"

typedef struct {
	zend_object_iterator it;
	bool by_ref;
	bool declared_props_done;
	zval declared_props;
	bool dynamic_props_done;
	uint32_t dynamic_prop_it;
	zval current_key;
	zval current_data;
} zend_hooked_object_iterator;

static zend_result zho_it_valid(zend_object_iterator *iter);
static void zho_it_move_forward(zend_object_iterator *iter);

static uint32_t zho_num_backed_props(zend_object *zobj)
{
	return zobj->ce->default_properties_count;
}

static zend_array *zho_build_properties_ex(zend_object *zobj, bool check_access, bool force_ptr, bool include_dynamic_props)
{
	zend_class_entry *ce = zobj->ce;
	zend_array *properties = zend_new_array(ce->default_properties_count);
	zend_hash_real_init_mixed(properties);

	/* Build list of parents */
	int32_t parent_count = 0;
	for (zend_class_entry *pce = ce; pce; pce = pce->parent) {
		parent_count++;
	}
	zend_class_entry **parents = emalloc(sizeof(zend_class_entry*) * parent_count);
	int32_t i = 0;
	for (zend_class_entry *pce = ce; pce; pce = pce->parent) {
		parents[i++] = pce;
	}

	/* Iterate parents top to bottom */
	i--;
	for (; i >= 0; i--) {
		zend_class_entry *pce = parents[i];

		zend_property_info *prop_info;
		ZEND_HASH_MAP_FOREACH_PTR(&pce->properties_info, prop_info) {
			if (prop_info->flags & ZEND_ACC_STATIC) {
				continue;
			}
			zend_string *property_name = prop_info->name;
			/* When promoting properties from protected to public, use the unmangled name to preserve order. */
			if (prop_info->flags & ZEND_ACC_PROTECTED) {
				const char *tmp = zend_get_unmangled_property_name(property_name);
				zend_string *unmangled_name = zend_string_init(tmp, strlen(tmp), false);
				zend_property_info *child_prop_info = zend_hash_find_ptr(&ce->properties_info, unmangled_name);
				if (child_prop_info && (child_prop_info->flags & ZEND_ACC_PUBLIC)) {
					property_name = unmangled_name;
				} else {
					zend_string_release(unmangled_name);
				}
			}
			if (check_access && zend_check_property_access(zobj, property_name, false) == FAILURE) {
				goto skip_property;
			}
			if (prop_info->hooks || force_ptr) {
				zend_hash_update_ptr(properties, property_name, prop_info);
			} else {
				if (UNEXPECTED(Z_TYPE_P(OBJ_PROP(zobj, prop_info->offset)) == IS_UNDEF)) {
					HT_FLAGS(properties) |= HASH_FLAG_HAS_EMPTY_IND;
				}
				zval *tmp = zend_hash_lookup(properties, property_name);
				ZVAL_INDIRECT(tmp, OBJ_PROP(zobj, prop_info->offset));
			}
skip_property:
			if (property_name != prop_info->name) {
				zend_string_release(property_name);
			}
		} ZEND_HASH_FOREACH_END();
	}

	efree(parents);

	if (include_dynamic_props && zobj->properties) {
		zend_string *prop_name;
		zval *prop_value;
		ZEND_HASH_FOREACH_STR_KEY_VAL_FROM(zobj->properties, prop_name, prop_value, zho_num_backed_props(zobj)) {
			Z_TRY_ADDREF_P(_zend_hash_append(properties, prop_name, prop_value));
		} ZEND_HASH_FOREACH_END();
	}

	return properties;
}

ZEND_API zend_array *zend_hooked_object_build_properties(zend_object *zobj)
{
	if (UNEXPECTED(zend_lazy_object_must_init(zobj))) {
		zobj = zend_lazy_object_init(zobj);
		if (UNEXPECTED(!zobj)) {
			return (zend_array*) &zend_empty_array;
		}
	}

	return zho_build_properties_ex(zobj, false, false, true);
}

static void zho_dynamic_it_init(zend_hooked_object_iterator *hooked_iter)
{
	zend_object *zobj = Z_OBJ_P(&hooked_iter->it.data);
	zend_array *properties = zobj->handlers->get_properties(zobj);
	hooked_iter->dynamic_props_done = false;
	hooked_iter->dynamic_prop_it = zend_hash_iterator_add(properties, zho_num_backed_props(zobj));
}

static void zho_it_get_current_key(zend_object_iterator *iter, zval *key);

static void zho_declared_it_fetch_current(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zend_object *zobj = Z_OBJ_P(&iter->data);
	zend_array *properties = Z_ARR(hooked_iter->declared_props);

	zend_property_info *prop_info = Z_PTR_P(zend_hash_get_current_data(properties));
	if (prop_info->hooks) {
		zend_function *get = prop_info->hooks[ZEND_PROPERTY_HOOK_GET];
		if (!get && (prop_info->flags & ZEND_ACC_VIRTUAL)) {
			return;
		}
		if (hooked_iter->by_ref
		 && (get == NULL
		  || !(get->common.fn_flags & ZEND_ACC_RETURN_REFERENCE))) {
			zend_throw_error(NULL, "Cannot create reference to property %s::$%s",
				ZSTR_VAL(zobj->ce->name), zend_get_unmangled_property_name(prop_info->name));
			return;
		}
		zend_string *unmangled_name = prop_info->name;
		if (ZSTR_VAL(unmangled_name)[0] == '\0') {
			const char *tmp = zend_get_unmangled_property_name(unmangled_name);
			unmangled_name = zend_string_init(tmp, strlen(tmp), false);
		}
		zval *value = zend_read_property_ex(prop_info->ce, zobj, unmangled_name, /* silent */ true, &hooked_iter->current_data);
		if (unmangled_name != prop_info->name) {
			zend_string_release(unmangled_name);
		}
		if (value == &EG(uninitialized_zval)) {
			return;
		} else if (value != &hooked_iter->current_data) {
			ZVAL_COPY(&hooked_iter->current_data, value);
		}
	} else {
		zval *property = OBJ_PROP(zobj, prop_info->offset);
		ZVAL_DEINDIRECT(property);
		if (Z_TYPE_P(property) == IS_UNDEF) {
			return;
		}
		if (!hooked_iter->by_ref) {
			ZVAL_DEREF(property);
		} else if (Z_TYPE_P(property) != IS_REFERENCE) {
			if (UNEXPECTED(prop_info->flags & ZEND_ACC_READONLY)) {
				zend_throw_error(NULL,
					"Cannot acquire reference to readonly property %s::$%s",
					ZSTR_VAL(prop_info->ce->name), zend_get_unmangled_property_name(prop_info->name));
				return;
			}
			ZVAL_MAKE_REF(property);
			if (ZEND_TYPE_IS_SET(prop_info->type)) {
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(property), prop_info);
			}
		}
		ZVAL_COPY(&hooked_iter->current_data, property);
	}

	if (ZSTR_VAL(prop_info->name)[0] == '\0') {
		const char *tmp = zend_get_unmangled_property_name(prop_info->name);
		ZVAL_STR(&hooked_iter->current_key, zend_string_init(tmp, strlen(tmp), false));
	} else {
		ZVAL_STR_COPY(&hooked_iter->current_key, prop_info->name);
	}
}

static void zho_dynamic_it_fetch_current(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zend_array *properties = Z_OBJ(iter->data)->properties;
	HashPosition pos = zend_hash_iterator_pos(hooked_iter->dynamic_prop_it, properties);

	if (pos >= properties->nNumUsed) {
		hooked_iter->dynamic_props_done = true;
		return;
	}

	Bucket *bucket = properties->arData + pos;

	if (UNEXPECTED(Z_TYPE(bucket->val) == IS_UNDEF)) {
		return;
	}

	zend_object *zobj = Z_OBJ_P(&hooked_iter->it.data);
	if (bucket->key && zend_check_property_access(zobj, bucket->key, true) != SUCCESS) {
		return;
	}

	if (hooked_iter->by_ref && Z_TYPE(bucket->val) != IS_REFERENCE) {
		ZVAL_MAKE_REF(&bucket->val);
	}
	ZVAL_COPY(&hooked_iter->current_data, &bucket->val);

	if (bucket->key) {
		ZVAL_STR_COPY(&hooked_iter->current_key, bucket->key);
	} else {
		ZVAL_LONG(&hooked_iter->current_key, bucket->h);
	}
}

static void zho_it_fetch_current(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	if (Z_TYPE(hooked_iter->current_data) != IS_UNDEF) {
		return;
	}

	while (true) {
		if (!hooked_iter->declared_props_done) {
			zho_declared_it_fetch_current(iter);
		} else if (!hooked_iter->dynamic_props_done) {
			zho_dynamic_it_fetch_current(iter);
		} else {
			break;
		}
		if (Z_TYPE(hooked_iter->current_data) != IS_UNDEF || EG(exception)) {
			break;
		}
		zho_it_move_forward(iter);
	}
}

static void zho_it_dtor(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zval_ptr_dtor(&iter->data);
	zval_ptr_dtor(&hooked_iter->declared_props);
	zval_ptr_dtor_nogc(&hooked_iter->current_key);
	zval_ptr_dtor(&hooked_iter->current_data);
	zend_hash_iterator_del(hooked_iter->dynamic_prop_it);
}

static zend_result zho_it_valid(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zho_it_fetch_current(iter);
	return Z_TYPE(hooked_iter->current_data) != IS_UNDEF ? SUCCESS : FAILURE;
}

static zval *zho_it_get_current_data(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zho_it_fetch_current(iter);
	return &hooked_iter->current_data;
}

static void zho_it_get_current_key(zend_object_iterator *iter, zval *key)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zho_it_fetch_current(iter);
	ZVAL_COPY(key, &hooked_iter->current_key);
}

static void zho_it_move_forward(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;

	zval_ptr_dtor(&hooked_iter->current_data);
	ZVAL_UNDEF(&hooked_iter->current_data);
	zval_ptr_dtor_nogc(&hooked_iter->current_key);
	ZVAL_UNDEF(&hooked_iter->current_key);

	if (!hooked_iter->declared_props_done) {
		zend_array *properties = Z_ARR(hooked_iter->declared_props);
		zend_hash_move_forward(properties);
		if (zend_hash_has_more_elements(properties) != SUCCESS) {
			hooked_iter->declared_props_done = true;
		}
	} else if (!hooked_iter->dynamic_props_done) {
		zend_array *properties = Z_OBJ(iter->data)->properties;
		HashPosition pos = zend_hash_iterator_pos(hooked_iter->dynamic_prop_it, properties);
		pos++;
		EG(ht_iterators)[hooked_iter->dynamic_prop_it].pos = pos;
	}
}

static void zho_it_rewind(zend_object_iterator *iter)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;

	zval_ptr_dtor(&hooked_iter->current_data);
	ZVAL_UNDEF(&hooked_iter->current_data);
	zval_ptr_dtor_nogc(&hooked_iter->current_key);
	ZVAL_UNDEF(&hooked_iter->current_key);

	zend_array *properties = Z_ARR(hooked_iter->declared_props);
	zend_hash_internal_pointer_reset(properties);
	hooked_iter->declared_props_done = !zend_hash_num_elements(properties);
	hooked_iter->dynamic_props_done = false;
	EG(ht_iterators)[hooked_iter->dynamic_prop_it].pos = zho_num_backed_props(Z_OBJ(iter->data));
}

static HashTable *zho_it_get_gc(zend_object_iterator *iter, zval **table, int *n)
{
	zend_hooked_object_iterator *hooked_iter = (zend_hooked_object_iterator*)iter;
	zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
	zend_get_gc_buffer_add_zval(gc_buffer, &iter->data);
	zend_get_gc_buffer_add_zval(gc_buffer, &hooked_iter->declared_props);
	zend_get_gc_buffer_add_zval(gc_buffer, &hooked_iter->current_data);
	zend_get_gc_buffer_use(gc_buffer, table, n);
	return NULL;
}

static const zend_object_iterator_funcs zend_hooked_object_it_funcs = {
	zho_it_dtor,
	zho_it_valid,
	zho_it_get_current_data,
	zho_it_get_current_key,
	zho_it_move_forward,
	zho_it_rewind,
	NULL,
	zho_it_get_gc,
};

ZEND_API zend_object_iterator *zend_hooked_object_get_iterator(zend_class_entry *ce, zval *object, int by_ref)
{
	zend_object *zobj = Z_OBJ_P(object);
	if (UNEXPECTED(zend_lazy_object_must_init(zobj))) {
		zobj = zend_lazy_object_init(zobj);
		if (UNEXPECTED(!zobj)) {
			return NULL;
		}
	}

	zend_hooked_object_iterator *iterator = emalloc(sizeof(zend_hooked_object_iterator));
	zend_iterator_init(&iterator->it);

	ZVAL_OBJ_COPY(&iterator->it.data, zobj);
	iterator->it.funcs = &zend_hooked_object_it_funcs;
	iterator->by_ref = by_ref;
	zend_array *properties = zho_build_properties_ex(zobj, true, true, false);
	ZVAL_ARR(&iterator->declared_props, properties);
	iterator->declared_props_done = !zend_hash_num_elements(properties);
	zho_dynamic_it_init(iterator);
	ZVAL_UNDEF(&iterator->current_key);
	ZVAL_UNDEF(&iterator->current_data);

	return &iterator->it;
}
