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
   | Authors: Derick Rethans <derick@php.net>                             |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_collection_arginfo.h"
#include "zend_execute.h"
#include "zend_extensions.h"
#include "zend_observer.h"

ZEND_API zend_class_entry *zend_ce_seq_collection;
ZEND_API zend_class_entry *zend_ce_dict_collection;
ZEND_API zend_object_handlers zend_seq_collection_object_handlers;
ZEND_API zend_object_handlers zend_dict_collection_object_handlers;

static int zend_implement_collection(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->ce_flags & ZEND_ACC_COLLECTION) {
		return SUCCESS;
	}

	zend_error_noreturn(E_ERROR, "Non-collection class %s cannot implement interface %s",
		ZSTR_VAL(class_type->name),
		ZSTR_VAL(interface->name));

	return FAILURE;
}

void zend_register_collection_ce(void)
{
	zend_ce_seq_collection = register_class_SeqCollection();
	zend_ce_seq_collection->interface_gets_implemented = zend_implement_collection;

	memcpy(&zend_seq_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_seq_collection_object_handlers.clone_obj = NULL;
	zend_seq_collection_object_handlers.compare = zend_objects_not_comparable;

	zend_ce_dict_collection = register_class_DictCollection();
	zend_ce_dict_collection->interface_gets_implemented = zend_implement_collection;

	memcpy(&zend_dict_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_dict_collection_object_handlers.clone_obj = NULL;
	zend_dict_collection_object_handlers.compare = zend_objects_not_comparable;
}

void zend_collection_add_interfaces(zend_class_entry *ce)
{
	uint32_t num_interfaces_before = ce->num_interfaces;

	ce->num_interfaces++;

	ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_RESOLVED_INTERFACES));

	ce->interface_names = erealloc(ce->interface_names, sizeof(zend_class_name) * ce->num_interfaces);

	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			ce->interface_names[num_interfaces_before].name = zend_string_copy(zend_ce_seq_collection->name);
			ce->interface_names[num_interfaces_before].lc_name = ZSTR_INIT_LITERAL("seqcollection", 0);
			ce->default_object_handlers = &zend_seq_collection_object_handlers;
			break;
		case ZEND_COLLECTION_DICT:
			ce->interface_names[num_interfaces_before].name = zend_string_copy(zend_ce_dict_collection->name);
			ce->interface_names[num_interfaces_before].lc_name = ZSTR_INIT_LITERAL("dictcollection", 0);
			ce->default_object_handlers = &zend_dict_collection_object_handlers;
			break;
	}

}

void zend_collection_register_handlers(zend_class_entry *ce)
{
	memcpy(&zend_seq_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_seq_collection_object_handlers.clone_obj = NULL;
	zend_seq_collection_object_handlers.compare = zend_objects_not_comparable;
	ce->default_object_handlers = &zend_seq_collection_object_handlers;

	memcpy(&zend_dict_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_dict_collection_object_handlers.clone_obj = NULL;
	zend_dict_collection_object_handlers.compare = zend_objects_not_comparable;
	ce->default_object_handlers = &zend_dict_collection_object_handlers;
}

static void seq_add_item(zend_object *object, zval *value);
static void dict_add_item(zend_object *object, zval *key, zval *value);

static ZEND_NAMED_FUNCTION(zend_collection_seq_add_func)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	seq_add_item(Z_OBJ_P(ZEND_THIS), value);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_add_func)
{
	zval *key, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	dict_add_item(Z_OBJ_P(ZEND_THIS), key, value);
}

static void zend_collection_register_func(zend_class_entry *ce, zend_known_string_id name_id, zend_internal_function *zif)
{
	zend_string *name = ZSTR_KNOWN(name_id);
	zif->type = ZEND_INTERNAL_FUNCTION;
	zif->module = EG(current_module);
	zif->scope = ce;
	zif->T = ZEND_OBSERVER_ENABLED;
    if (EG(active)) { // at run-time
		ZEND_MAP_PTR_INIT(zif->run_time_cache, zend_arena_calloc(&CG(arena), 1, zend_internal_run_time_cache_reserved_size()));
	} else {
		ZEND_MAP_PTR_NEW(zif->run_time_cache);
	}

	if (!zend_hash_add_ptr(&ce->function_table, name, zif)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::%s()", ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}
}


void zend_collection_register_funcs(zend_class_entry *ce)
{
	const uint32_t fn_flags = ZEND_ACC_PUBLIC|ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_ARENA_ALLOCATED;

	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			zend_internal_function *seq_add_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
			seq_add_function->handler = zend_collection_seq_add_func;
			seq_add_function->function_name = ZSTR_KNOWN(ZEND_STR_ADD);
			seq_add_function->fn_flags = fn_flags;
			seq_add_function->num_args = 1;
			seq_add_function->required_num_args = 1;
			seq_add_function->arg_info = (zend_internal_arg_info *) (arginfo_class_SeqCollection_add + 1);
			zend_collection_register_func(ce, ZEND_STR_ADD, seq_add_function);
			break;
		case ZEND_COLLECTION_DICT:
			zend_internal_function *dict_add_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
			dict_add_function->handler = zend_collection_dict_add_func;
			dict_add_function->function_name = ZSTR_KNOWN(ZEND_STR_ADD);
			dict_add_function->fn_flags = fn_flags;
			dict_add_function->num_args = 2;
			dict_add_function->required_num_args = 2;
			dict_add_function->arg_info = (zend_internal_arg_info *) (arginfo_class_DictCollection_add + 1);
			zend_collection_register_func(ce, ZEND_STR_ADD, dict_add_function);
			break;
	}
}

void zend_collection_register_props(zend_class_entry *ce)
{
	ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zval name_default_value;
	ZVAL_UNDEF(&name_default_value);
	zend_type name_type = ZEND_TYPE_INIT_CODE(IS_ARRAY, 0, 0);
	zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_VALUE), &name_default_value, ZEND_ACC_PUBLIC | ZEND_ACC_READONLY, NULL, name_type);
}

static void create_array_if_needed(zend_class_entry *ce, zend_object *object)
{
	zval *value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	if (Z_TYPE_P(value_prop) == IS_ARRAY) {
		return;
	}

	zval new_array;

	array_init(&new_array);

	zend_update_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), &new_array);

	zval_ptr_dtor(&new_array);
}

static void seq_add_item(zend_object *object, zval *value)
{
	zend_class_entry *ce = object->ce;
	zval rv;
	zval *value_prop;

	create_array_if_needed(ce, object);
	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
	Z_ADDREF_P(value);
	add_next_index_zval(value_prop, value);
}

static void dict_add_item(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	zval rv;
	zval *value_prop;

	if (ce->collection_key_type == IS_LONG && Z_TYPE_P(offset) == IS_LONG) {
		create_array_if_needed(ce, object);
		value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
		Z_ADDREF_P(value);
		add_index_zval(value_prop, Z_LVAL_P(offset), value);
		return;
	} else if (ce->collection_key_type == IS_STRING && Z_TYPE_P(offset) == IS_STRING) {
		create_array_if_needed(ce, object);
		value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
		Z_ADDREF_P(value);
		add_assoc_zval_ex(value_prop, Z_STRVAL_P(offset), Z_STRLEN_P(offset), value);
		return;
	} else {
		zend_type_error(
			"Key type %s of element does not match collection key type %s",
			offset ? zend_zval_type_name(offset) : zend_get_type_by_const(IS_NULL),
			zend_get_type_by_const(ce->collection_key_type)
		);
	}
}


void zend_collection_add_item(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_COLLECTION);

	if (offset && ce->collection_data_structure == ZEND_COLLECTION_SEQ) {
		zend_value_error("Specifying an offset for sequence collections is not allowed");
		return;
	}

	if (!offset && ce->collection_data_structure == ZEND_COLLECTION_DICT) {
		zend_value_error("Specifying an offset for dictionary collections is required");
		return;
	}

	if (!zend_check_type(&ce->collection_item_type, value, NULL, ce, 0, false)) {
		zend_string *type_str = zend_type_to_string(ce->collection_item_type);
		zend_type_error(
			"Value type %s does not match collection item type %s",
			zend_zval_type_name(value),
			ZSTR_VAL(type_str)
		);
		zend_string_release(type_str);
		return;
	}

	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			seq_add_item(object, value);
			break;
		case ZEND_COLLECTION_DICT:
			dict_add_item(object, offset, value);
			break;
	}
}

static int key_type_allowed(zend_class_entry *ce, zval *offset)
{
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_COLLECTION);

	if (ce->collection_key_type != Z_TYPE_P(offset)) {
		zend_type_error(
			"Key type %s of element does not match collection key type %s",
			offset ? zend_zval_type_name(offset) : zend_get_type_by_const(IS_NULL),
			zend_get_type_by_const(ce->collection_key_type)
		);
		return false;
	}

	return true;
}

int zend_collection_has_item(zend_object *object, zval *offset)
{
	zval rv;
	zval *value_prop;
	zend_class_entry *ce = object->ce;

	if (!key_type_allowed(ce, offset)) {
		return false;
	}

	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);

	if (Z_TYPE_P(offset) == IS_STRING) {
		return zend_hash_find(HASH_OF(value_prop), Z_STR_P(offset)) != NULL;
	} else {
		return zend_hash_index_find(HASH_OF(value_prop), Z_LVAL_P(offset)) != NULL;
	}

	return false;
}

zval *zend_collection_read_item(zend_object *object, zval *offset)
{
	zval rv;
	zval *value_prop, *value;
	zend_class_entry *ce = object->ce;

	if (!key_type_allowed(ce, offset)) {
		return NULL;
	}

	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);

	if (Z_TYPE_P(offset) == IS_STRING) {
		value = zend_hash_find(HASH_OF(value_prop), Z_STR_P(offset));
	} else {
		value = zend_hash_index_find(HASH_OF(value_prop), Z_LVAL_P(offset));
	}

	return value;
}

void zend_collection_unset_item(zend_object *object, zval *offset)
{
	zval rv;
	zval *value_prop;
	zend_class_entry *ce = object->ce;

	if (!key_type_allowed(ce, offset)) {
		return;
	}

	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);

	if (Z_TYPE_P(offset) == IS_STRING) {
		zend_hash_del(HASH_OF(value_prop), Z_STR_P(offset));
	} else {
		zend_hash_index_del(HASH_OF(value_prop), Z_LVAL_P(offset));
	}
}
