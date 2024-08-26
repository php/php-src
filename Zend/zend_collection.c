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
#include "zend_exceptions.h"
#include "zend_extensions.h"
#include "zend_observer.h"

#include "ext/spl/spl_exceptions.h"

ZEND_API zend_class_entry *zend_ce_seq_collection;
ZEND_API zend_class_entry *zend_ce_dict_collection;
ZEND_API zend_object_handlers zend_seq_collection_object_handlers;
ZEND_API zend_object_handlers zend_dict_collection_object_handlers;

static int seq_collection_compare(zval *object1, zval *object2);
static int dict_collection_compare(zval *object1, zval *object2);

static zend_result seq_collection_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2);

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
	zend_seq_collection_object_handlers.compare = seq_collection_compare;
	zend_seq_collection_object_handlers.do_operation = seq_collection_do_operation;

	zend_ce_dict_collection = register_class_DictCollection();
	zend_ce_dict_collection->interface_gets_implemented = zend_implement_collection;

	memcpy(&zend_dict_collection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_dict_collection_object_handlers.clone_obj = NULL;
	zend_dict_collection_object_handlers.compare = dict_collection_compare;
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

bool zend_collection_add_item(zend_object *object, zval *offset, zval *value);
void zend_collection_set_item(zend_object *object, zval *offset, zval *value);
int zend_collection_has_item(zend_object *object, zval *offset);
zval *zend_collection_read_item(zend_object *object, zval *offset);
void zend_collection_unset_item(zend_object *object, zval *offset);

static void create_array_if_needed(zend_class_entry *ce, zend_object *object);

static const char *get_data_structure_name(zend_class_entry *ce)
{
	if (ce->collection_data_structure == ZEND_COLLECTION_SEQ) {
		return "sequence";
	}
	if (ce->collection_data_structure == ZEND_COLLECTION_DICT) {
		return "dictionary";
	}
	return "unknown";
}

static int key_type_allowed(zend_class_entry *ce, zval *offset)
{
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_COLLECTION);

	if (Z_TYPE_P(offset) != IS_LONG && Z_TYPE_P(offset) != IS_STRING) {
		zend_type_error(
			"Key type %s is not allowed for %s %s",
			ZSTR_VAL(ce->name),
			offset ? zend_zval_type_name(offset) : zend_get_type_by_const(IS_NULL),
			get_data_structure_name(ce)
		);
	}

	if (ce->collection_key_type != Z_TYPE_P(offset)) {
		zend_type_error(
			"Key type %s of element does not match %s %s key type %s",
			offset ? zend_zval_type_name(offset) : zend_get_type_by_const(IS_NULL),
			ZSTR_VAL(ce->name),
			get_data_structure_name(ce),
			zend_get_type_by_const(ce->collection_key_type)
		);
		return false;
	}

	return true;
}

static int collection_is_equal_function(zval *z1, zval *z2)
{
	zval result;
	is_equal_function(&result, z1, z2);

	return Z_TYPE(result) == IS_TRUE ? 0 : 1;
}

static int seq_collection_compare(zval *object1, zval *object2)
{
	zval *value_prop_object1, *value_prop_object2;

	if (!object1 || !object2) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE_P(object1) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(object1), zend_ce_seq_collection)) {
		return 1;
	}
	if (Z_TYPE_P(object2) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(object2), zend_ce_seq_collection)) {
		return 1;
	}

	if (Z_OBJ_P(object1) == Z_OBJ_P(object2)) {
		return 0;
	}

	value_prop_object1 = zend_read_property_ex(Z_OBJCE_P(object1), Z_OBJ_P(object1), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);
	value_prop_object2 = zend_read_property_ex(Z_OBJCE_P(object2), Z_OBJ_P(object2), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	if (!value_prop_object1 || !value_prop_object2) {
		return 1;
	}

	return (zend_hash_compare(
		Z_ARRVAL_P(value_prop_object1),
		Z_ARRVAL_P(value_prop_object2),
		(compare_func_t) collection_is_equal_function, true
	) != 0);
}

static void seq_copy_add_elements(zend_object *clone, zval *other)
{
	zend_class_entry *ce = clone->ce;
	zval *value_prop;
	zval *element;

	value_prop = zend_read_property_ex(ce, Z_OBJ_P(other), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value_prop), element) {
		if (!zend_collection_add_item(clone, NULL, element)) {
			return;
		}
	} ZEND_HASH_FOREACH_END();
}

static zend_result seq_collection_do_operation_add_impl(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	zend_object *clone;

	clone = zend_objects_clone_obj(Z_OBJ_P(op1));

	seq_copy_add_elements(clone, op2);

	ZVAL_OBJ(result, clone);

	return SUCCESS;
}

static zend_result seq_collection_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	if (Z_TYPE_P(op1) != IS_OBJECT || Z_TYPE_P(op2) != IS_OBJECT) {
		return FAILURE;
	}

	if (!instanceof_function(Z_OBJCE_P(op1), zend_ce_seq_collection) || !instanceof_function(Z_OBJCE_P(op2), zend_ce_seq_collection)) {
		return FAILURE;
	}

	switch (opcode) {
		case ZEND_ADD:
			return seq_collection_do_operation_add_impl(opcode, result, op1, op2);
	}
	return FAILURE;
}

static int dict_collection_compare(zval *object1, zval *object2)
{
	zval *value_prop_object1, *value_prop_object2;

	if (!object1 || !object2) {
		return ZEND_UNCOMPARABLE;
	}

	if (Z_TYPE_P(object1) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(object1), zend_ce_dict_collection)) {
		return 1;
	}
	if (Z_TYPE_P(object2) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(object2), zend_ce_dict_collection)) {
		return 1;
	}

	if (Z_OBJ_P(object1) == Z_OBJ_P(object2)) {
		return 0;
	}

	value_prop_object1 = zend_read_property_ex(Z_OBJCE_P(object1), Z_OBJ_P(object1), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);
	value_prop_object2 = zend_read_property_ex(Z_OBJCE_P(object2), Z_OBJ_P(object2), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	if (!value_prop_object1 || !value_prop_object2) {
		return 1;
	}

	return (zend_hash_compare(
		Z_ARRVAL_P(value_prop_object1),
		Z_ARRVAL_P(value_prop_object2),
		(compare_func_t) collection_is_equal_function, false
	) != 0);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_add_func)
{
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_collection_add_item(Z_OBJ_P(ZEND_THIS), NULL, value);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_remove_func)
{
	zval *index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	zend_collection_unset_item(Z_OBJ_P(ZEND_THIS), index);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_has_func)
{
	zval *index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_collection_has_item(Z_OBJ_P(ZEND_THIS), index));
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_get_func)
{
	zval *index, *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	value = zend_collection_read_item(Z_OBJ_P(ZEND_THIS), index);
	if (!value) {
		return;
	}

	RETURN_COPY_VALUE(value);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_with_func)
{
	zval *value;
	zend_object *clone;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	zend_collection_add_item(clone, NULL, value);

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_without_func)
{
	zval *index;
	zend_object *clone;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	zend_collection_unset_item(clone, index);

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_set_func)
{
	zval *index;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(index)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (!key_type_allowed(Z_OBJCE_P(ZEND_THIS), index)) {
		return;
	}

	if (!zend_collection_has_item(Z_OBJ_P(ZEND_THIS), index)) {
		zend_throw_exception_ex(
			spl_ce_OutOfBoundsException, 0,
			"Index '%ld' does not exist in the %s sequence",
			Z_LVAL_P(index), ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name)
		);
	}

	zend_collection_set_item(Z_OBJ_P(ZEND_THIS), index, value);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_concat_func)
{
	zval *other;
	zend_object *clone;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, zend_ce_seq_collection);
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	seq_copy_add_elements(clone, other);

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_equals_func)
{
	zval *other;
	zval *value_prop_us, *value_prop_other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, zend_ce_seq_collection);
	ZEND_PARSE_PARAMETERS_END();

	if (Z_OBJ_P(ZEND_THIS) == Z_OBJ_P(other)) {
		RETURN_TRUE;
	}

	value_prop_us = zend_read_property_ex(Z_OBJCE_P(ZEND_THIS), Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);
	value_prop_other = zend_read_property_ex(Z_OBJCE_P(other), Z_OBJ_P(other), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	if (!value_prop_us || !value_prop_other) {
		RETURN_FALSE;
	}

	RETURN_BOOL(zend_hash_compare(Z_ARRVAL_P(value_prop_us), Z_ARRVAL_P(value_prop_other), (compare_func_t) collection_is_equal_function, true) == 0);
}

static ZEND_NAMED_FUNCTION(zend_collection_seq_map_func)
{
	zend_object *object = Z_OBJ_P(ZEND_THIS);
	zend_class_entry *ce = Z_OBJCE_P(ZEND_THIS);
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zval *value_prop;
	zend_string *type;
	zval *operand;
	zval retval;
	zval args[1];

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_STR(type)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry *return_ce = zend_lookup_class(type);
	if (!return_ce) {
		zend_type_error(
			"Type '%s' can not be fetched",
			ZSTR_VAL(type)
		);
		return;
	}

	if (!instanceof_function(return_ce, zend_ce_seq_collection)) {
		zend_type_error(
			"Type '%s' must implement SeqCollection interface",
			ZSTR_VAL(type)
		);
		return;
	}

	create_array_if_needed(ce, object);
	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	object_init_ex(return_value, return_ce);
	Z_OBJCE_P(return_value)->collection_data_structure = return_ce->collection_data_structure;

	fci.retval = &retval;
	fci.param_count = 1;

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value_prop), operand) {
		ZVAL_COPY(&args[0], operand);

		fci.params = args;

		if (zend_call_function(&fci, &fci_cache) == SUCCESS) {
			zval_ptr_dtor(&args[0]);

			if (!zend_collection_add_item(Z_OBJ_P(return_value), NULL, &retval)) {
				return;
			}
		} else {
			zval_ptr_dtor(&args[0]);

			return;
		}

	} ZEND_HASH_FOREACH_END();
}


static ZEND_NAMED_FUNCTION(zend_collection_dict_add_func)
{
	zval *key, *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	zend_collection_add_item(Z_OBJ_P(ZEND_THIS), key, value);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_remove_func)
{
	zval *index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	zend_collection_unset_item(Z_OBJ_P(ZEND_THIS), index);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_has_func)
{
	zval *index;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(zend_collection_has_item(Z_OBJ_P(ZEND_THIS), index));
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_get_func)
{
	zval *index, *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	value = zend_collection_read_item(Z_OBJ_P(ZEND_THIS), index);

	RETURN_COPY_VALUE(value);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_with_func)
{
	zval *index, *value;
	zend_object *clone;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(index)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	zend_collection_add_item(clone, index, value);

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_without_func)
{
	zval *index;
	zend_object *clone;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(index)
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	zend_collection_unset_item(clone, index);

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_set_func)
{
	zval *index;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(index)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	if (!key_type_allowed(Z_OBJCE_P(ZEND_THIS), index)) {
		return;
	}

	if (!zend_collection_has_item(Z_OBJ_P(ZEND_THIS), index)) {
		switch (Z_TYPE_P(index)) {
			case IS_LONG:
				zend_throw_exception_ex(
					spl_ce_OutOfBoundsException, 0,
					"Index '%ld' does not exist in the %s dictionary",
					Z_LVAL_P(index), ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name)
				);
				return;
			case IS_STRING:
				zend_throw_exception_ex(
					spl_ce_OutOfBoundsException, 0,
					"Index '%s' does not exist in the %s dictionary",
					Z_STRVAL_P(index), ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name)
					);
				return;
		}
	}

	zend_collection_set_item(Z_OBJ_P(ZEND_THIS), index, value);

	RETURN_OBJ_COPY(Z_OBJ_P(ZEND_THIS));
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_concat_func)
{
	zval *other;
	zend_object *clone;
	zend_class_entry *ce = Z_OBJCE_P(ZEND_THIS);
	zval *value_prop;
	zval *element;
	zend_ulong int_key;
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, zend_ce_dict_collection);
	ZEND_PARSE_PARAMETERS_END();

	clone = zend_objects_clone_obj(Z_OBJ_P(ZEND_THIS));

	value_prop = zend_read_property_ex(ce, Z_OBJ_P(other), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(value_prop), int_key, key, element) {
		zval zkey;

		if (key) {
			ZVAL_STR(&zkey, key);
		} else {
			ZVAL_LONG(&zkey, int_key);
		}

		if (!zend_collection_add_item(clone, &zkey, element)) {
			return;
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_OBJ(clone);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_equals_func)
{
	zval *other;
	zval *value_prop_us, *value_prop_other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(other, zend_ce_dict_collection);
	ZEND_PARSE_PARAMETERS_END();

	if (Z_OBJ_P(ZEND_THIS) == Z_OBJ_P(other)) {
		RETURN_TRUE;
	}

	value_prop_us = zend_read_property_ex(Z_OBJCE_P(ZEND_THIS), Z_OBJ_P(ZEND_THIS), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);
	value_prop_other = zend_read_property_ex(Z_OBJCE_P(other), Z_OBJ_P(other), ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	if (!value_prop_us || !value_prop_other) {
		RETURN_FALSE;
	}

	RETURN_BOOL(zend_hash_compare(Z_ARRVAL_P(value_prop_us), Z_ARRVAL_P(value_prop_other), (compare_func_t) collection_is_equal_function, false) == 0);
}

static ZEND_NAMED_FUNCTION(zend_collection_dict_map_func)
{
	zend_object *object = Z_OBJ_P(ZEND_THIS);
	zend_class_entry *ce = Z_OBJCE_P(ZEND_THIS);
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zval *value_prop;
	zend_string *type;
	zval *operand;
	zend_ulong index;
	zend_string *key;
	zval retval;
	zval args[2];

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_STR(type)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry *return_ce = zend_lookup_class(type);
	if (!return_ce) {
		zend_type_error(
			"Type '%s' can not be fetched",
			ZSTR_VAL(type)
		);
		return;
	}

	if (!instanceof_function(return_ce, zend_ce_dict_collection)) {
		zend_type_error(
			"Type '%s' must implement DictCollection interface",
			ZSTR_VAL(type)
		);
		return;
	}

	create_array_if_needed(ce, object);
	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, NULL);

	object_init_ex(return_value, return_ce);
	Z_OBJCE_P(return_value)->collection_data_structure = return_ce->collection_data_structure;

	fci.retval = &retval;
	fci.param_count = 2;

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(value_prop), index, key, operand) {
		ZVAL_COPY(&args[0], operand);
		if (key != NULL) {
			ZVAL_STR_COPY(&args[1], key);
		} else {
			ZVAL_LONG(&args[1], index);
		}

		fci.params = args;

		if (zend_call_function(&fci, &fci_cache) == SUCCESS) {
			bool ok = zend_collection_add_item(Z_OBJ_P(return_value), &args[1], &retval);

			zval_ptr_dtor(&args[0]);
			zval_ptr_dtor(&args[1]);

			if (!ok) {
				return;
			}
		} else {
			zval_ptr_dtor(&args[0]);
			zval_ptr_dtor(&args[1]);

			return;
		}

	} ZEND_HASH_FOREACH_END();
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

#define REGISTER_FUNCTION(name, php_handler, arginfo, argc) \
	{ \
		zend_internal_function *zif_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1); \
		zif_function->handler = (php_handler); \
		zif_function->function_name = ZSTR_KNOWN((name)); \
		zif_function->fn_flags = fn_flags; \
		zif_function->num_args = (argc); \
		zif_function->required_num_args = (argc); \
		zif_function->arg_info = (zend_internal_arg_info *) ((arginfo) + 1); \
		zend_collection_register_func(ce, (name), zif_function); \
	}


void zend_collection_register_funcs(zend_class_entry *ce)
{
	const uint32_t fn_flags = ZEND_ACC_PUBLIC|ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_ARENA_ALLOCATED;

	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			REGISTER_FUNCTION(ZEND_STR_ADD, zend_collection_seq_add_func, arginfo_class_SeqCollection_add, 1);
			REGISTER_FUNCTION(ZEND_STR_REMOVE, zend_collection_seq_remove_func, arginfo_class_SeqCollection_remove, 1);
			REGISTER_FUNCTION(ZEND_STR_HAS, zend_collection_seq_has_func, arginfo_class_SeqCollection_has, 1);
			REGISTER_FUNCTION(ZEND_STR_GET, zend_collection_seq_get_func, arginfo_class_SeqCollection_get, 1);
			REGISTER_FUNCTION(ZEND_STR_WITH, zend_collection_seq_with_func, arginfo_class_SeqCollection_with, 1);
			REGISTER_FUNCTION(ZEND_STR_WITHOUT, zend_collection_seq_without_func, arginfo_class_SeqCollection_without, 1);
			REGISTER_FUNCTION(ZEND_STR_SET, zend_collection_seq_set_func, arginfo_class_SeqCollection_set, 2);
			REGISTER_FUNCTION(ZEND_STR_CONCAT, zend_collection_seq_concat_func, arginfo_class_SeqCollection_concat, 1);
			REGISTER_FUNCTION(ZEND_STR_EQUALS, zend_collection_seq_equals_func, arginfo_class_SeqCollection_equals, 1);
			REGISTER_FUNCTION(ZEND_STR_MAP, zend_collection_seq_map_func, arginfo_class_SeqCollection_map, 2);
			break;
		case ZEND_COLLECTION_DICT:
			REGISTER_FUNCTION(ZEND_STR_ADD, zend_collection_dict_add_func, arginfo_class_DictCollection_add, 2);
			REGISTER_FUNCTION(ZEND_STR_REMOVE, zend_collection_dict_remove_func, arginfo_class_DictCollection_remove, 1);
			REGISTER_FUNCTION(ZEND_STR_HAS, zend_collection_dict_has_func, arginfo_class_DictCollection_has, 1);
			REGISTER_FUNCTION(ZEND_STR_GET, zend_collection_dict_get_func, arginfo_class_DictCollection_get, 1);
			REGISTER_FUNCTION(ZEND_STR_WITH, zend_collection_dict_with_func, arginfo_class_DictCollection_with, 2);
			REGISTER_FUNCTION(ZEND_STR_WITHOUT, zend_collection_dict_without_func, arginfo_class_DictCollection_without, 1);
			REGISTER_FUNCTION(ZEND_STR_SET, zend_collection_dict_set_func, arginfo_class_DictCollection_set, 2);
			REGISTER_FUNCTION(ZEND_STR_CONCAT, zend_collection_dict_concat_func, arginfo_class_DictCollection_concat, 1);
			REGISTER_FUNCTION(ZEND_STR_EQUALS, zend_collection_dict_equals_func, arginfo_class_DictCollection_equals, 1);
			REGISTER_FUNCTION(ZEND_STR_MAP, zend_collection_dict_map_func, arginfo_class_DictCollection_map, 2);
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
	SEPARATE_ARRAY(value_prop);
	zval_add_ref(value);
	add_next_index_zval(value_prop, value);
}

static void seq_set_item(zend_object *object, zend_long index, zval *value)
{
	zend_class_entry *ce = object->ce;
	zval rv;
	zval *value_prop;

	create_array_if_needed(ce, object);
	value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
	SEPARATE_ARRAY(value_prop);
	zval_add_ref(value);
	add_index_zval(value_prop, index, value);
}

static void dict_add_or_set_item(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	zval rv;
	zval *value_prop;

	if (!key_type_allowed(ce, offset)) {
		return;
	}

	switch (ce->collection_key_type) {
		case IS_LONG: {
			create_array_if_needed(ce, object);
			value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
			SEPARATE_ARRAY(value_prop);
			zval_add_ref(value);
			add_index_zval(value_prop, Z_LVAL_P(offset), value);
			break;
		}
		case IS_STRING: {
			create_array_if_needed(ce, object);
			value_prop = zend_read_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), true, &rv);
			SEPARATE_ARRAY(value_prop);
			zval_add_ref(value);
			add_assoc_zval_ex(value_prop, Z_STRVAL_P(offset), Z_STRLEN_P(offset), value);
			break;
		}
	}
}


bool zend_collection_add_item(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_COLLECTION);

	if (offset && ce->collection_data_structure == ZEND_COLLECTION_SEQ) {
		zend_value_error("Specifying an offset for sequence collections is not allowed");
		return false;
	}

	if (!offset && ce->collection_data_structure == ZEND_COLLECTION_DICT) {
		zend_value_error("Specifying an offset for dictionary collections is required");
		return false;
	}

	if (!zend_check_type(&ce->collection_item_type, value, NULL, ce, true, true)) {
		zend_string *type_str = zend_type_to_string(ce->collection_item_type);
		zend_type_error(
			"Value type %s does not match %s collection item type %s",
			ZSTR_VAL(ce->name),
			zend_zval_type_name(value),
			ZSTR_VAL(type_str)
		);
		zend_string_release(type_str);
		return false;
	}

	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			seq_add_item(object, value);
			break;
		case ZEND_COLLECTION_DICT:
			dict_add_or_set_item(object, offset, value);
			break;
	}

	return true;
}

void zend_collection_set_item(zend_object *object, zval *offset, zval *value)
{
	zend_class_entry *ce = object->ce;
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_COLLECTION);

	if (Z_TYPE_P(offset) != IS_LONG && ce->collection_data_structure == ZEND_COLLECTION_SEQ) {
		zend_value_error("Specifying a non-integer offset for sequence collections is not allowed");
		return;
	}

	if (!((Z_TYPE_P(offset) == IS_LONG || Z_TYPE_P(offset) == IS_STRING)) && ce->collection_data_structure == ZEND_COLLECTION_DICT) {
		zend_value_error("Specifying a non-integer/non-string offset for sequence collections is not allowed");
		return;
	}

	if (!zend_check_type(&ce->collection_item_type, value, NULL, ce, 0, false)) {
		zend_string *type_str = zend_type_to_string(ce->collection_item_type);
		zend_type_error(
			"Value type %s does not match %s collection item type %s",
			ZSTR_VAL(ce->name),
			zend_zval_type_name(value),
			ZSTR_VAL(type_str)
		);
		zend_string_release(type_str);
		return;
	}



	switch (ce->collection_data_structure) {
		case ZEND_COLLECTION_SEQ:
			seq_set_item(object, Z_LVAL_P(offset), value);
			break;
		case ZEND_COLLECTION_DICT:
			dict_add_or_set_item(object, offset, value);
			break;
	}
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
	SEPARATE_ARRAY(value_prop);

	// TODO: Should unset throw when an item with key 'offset' does not exist?

	if (Z_TYPE_P(offset) == IS_STRING) {
		zend_hash_del(HASH_OF(value_prop), Z_STR_P(offset));
	} else {
		zend_array *new_array;
		zval new_zval;

		zend_hash_index_del(HASH_OF(value_prop), Z_LVAL_P(offset));

		if (!HT_IS_WITHOUT_HOLES(HASH_OF(value_prop))) {
			new_array = zend_array_to_list(HASH_OF(value_prop));
			ZVAL_ARR(&new_zval, new_array);
			Z_PROP_FLAG_P(value_prop) |= IS_PROP_REINITABLE;
			zend_update_property_ex(ce, object, ZSTR_KNOWN(ZEND_STR_VALUE), &new_zval);
			Z_PROP_FLAG_P(value_prop) &= IS_PROP_REINITABLE;
		}
	}
}
