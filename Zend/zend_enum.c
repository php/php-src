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
#include "zend_compile.h"
#include "zend_interfaces.h"
#include "zend_inheritance.h"
#include "zend_constants.h"
#include "zend_enum_arginfo.h"

#define ZEND_ENUM_PROPERTY_ERROR() \
	zend_throw_error(NULL, "Enum properties are immutable")

#define ZEND_ENUM_DISALLOW_MAGIC_METHOD(propertyName, methodName) \
	do { \
		if (ce->propertyName) { \
			zend_error_noreturn(E_COMPILE_ERROR, "Enum may not include %s", methodName); \
		} \
	} while (0);

ZEND_API zend_class_entry *zend_ce_unit_enum;
ZEND_API zend_class_entry *zend_ce_backed_enum;

static zend_object_handlers enum_handlers;

static void enum_obj_init(zend_object *obj, zend_string *case_name, zval *backing_value_zv)
{
	ZVAL_STR_COPY(OBJ_PROP_NUM(obj, 0), case_name);
	if (backing_value_zv != NULL) {
		ZVAL_COPY(OBJ_PROP_NUM(obj, 1), backing_value_zv);
	}

	obj->handlers = &enum_handlers;
}

zend_object *zend_enum_new(zval *result, zend_class_entry *ce, zend_string *case_name, zval *backing_value_zv)
{
	zend_object *obj = zend_objects_new(ce);
	ZVAL_OBJ(result, obj);
	enum_obj_init(obj, case_name, backing_value_zv);
	return obj;
}

static void zend_verify_enum_properties(zend_class_entry *ce)
{
	zend_property_info *property_info;

	ZEND_HASH_FOREACH_PTR(&ce->properties_info, property_info) {
		if (zend_string_equals_literal(property_info->name, "name")) {
			continue;
		}
		if (
			ce->enum_backing_type != IS_UNDEF
			&& zend_string_equals_literal(property_info->name, "value")
		) {
			continue;
		}
		// FIXME: File/line number for traits?
		zend_error_noreturn(E_COMPILE_ERROR, "Enum \"%s\" may not include properties",
			ZSTR_VAL(ce->name));
	} ZEND_HASH_FOREACH_END();
}

static void zend_verify_enum_magic_methods(zend_class_entry *ce)
{
	// Only __get, __call and __invoke are allowed

	ZEND_ENUM_DISALLOW_MAGIC_METHOD(constructor, "__construct");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(destructor, "__destruct");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(clone, "__clone");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__get, "__get");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__set, "__set");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__unset, "__unset");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__isset, "__isset");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__tostring, "__toString");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__debugInfo, "__debugInfo");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__serialize, "__serialize");
	ZEND_ENUM_DISALLOW_MAGIC_METHOD(__unserialize, "__unserialize");

	const char *forbidden_methods[] = {
		"__sleep",
		"__wakeup",
		"__set_state",
	};

	uint32_t forbidden_methods_length = sizeof(forbidden_methods) / sizeof(forbidden_methods[0]);
	for (uint32_t i = 0; i < forbidden_methods_length; ++i) {
		const char *forbidden_method = forbidden_methods[i];

		if (zend_hash_str_exists(&ce->function_table, forbidden_method, strlen(forbidden_method))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Enum may not include magic method %s", forbidden_method);
		}
	}
}

static void zend_verify_enum_interfaces(zend_class_entry *ce)
{
	if (zend_class_implements_interface(ce, zend_ce_serializable)) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Enums may not implement the Serializable interface");
	}
}

void zend_verify_enum(zend_class_entry *ce)
{
	zend_verify_enum_properties(ce);
	zend_verify_enum_magic_methods(ce);
	zend_verify_enum_interfaces(ce);
}

static zval *zend_enum_read_property(zend_object *zobj, zend_string *name, int type, void **cache_slot, zval *rv) /* {{{ */
{
	if (type == BP_VAR_W || type == BP_VAR_RW || type == BP_VAR_UNSET) {
		zend_throw_error(NULL, "Cannot acquire reference to property %s::$%s", ZSTR_VAL(zobj->ce->name), ZSTR_VAL(name));
		return &EG(uninitialized_zval);
	}

	return zend_std_read_property(zobj, name, type, cache_slot, rv);
}

static ZEND_COLD zval *zend_enum_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
	ZEND_ENUM_PROPERTY_ERROR();
	return &EG(uninitialized_zval);
}

static ZEND_COLD void zend_enum_unset_property(zend_object *object, zend_string *member, void **cache_slot)
{
	ZEND_ENUM_PROPERTY_ERROR();
}

static zval *zend_enum_get_property_ptr_ptr(zend_object *zobj, zend_string *name, int type, void **cache_slot)
{
	return NULL;
}

static int zend_implement_unit_enum(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (class_type->ce_flags & ZEND_ACC_ENUM) {
		return SUCCESS;
	}

	zend_error_noreturn(E_ERROR, "Non-enum class %s cannot implement interface %s",
		ZSTR_VAL(class_type->name),
		ZSTR_VAL(interface->name));

	return FAILURE;
}

static int zend_implement_backed_enum(zend_class_entry *interface, zend_class_entry *class_type)
{
	if (!(class_type->ce_flags & ZEND_ACC_ENUM)) {
		zend_error_noreturn(E_ERROR, "Non-enum class %s cannot implement interface %s",
			ZSTR_VAL(class_type->name),
			ZSTR_VAL(interface->name));
		return FAILURE;
	}

	if (class_type->enum_backing_type == IS_UNDEF) {
		zend_error_noreturn(E_ERROR, "Non-backed enum %s cannot implement interface %s",
			ZSTR_VAL(class_type->name),
			ZSTR_VAL(interface->name));
		return FAILURE;
	}

	return SUCCESS;
}

void zend_register_enum_ce(void)
{
	zend_ce_unit_enum = register_class_UnitEnum();
	zend_ce_unit_enum->interface_gets_implemented = zend_implement_unit_enum;

	zend_ce_backed_enum = register_class_BackedEnum(zend_ce_unit_enum);
	zend_ce_backed_enum->interface_gets_implemented = zend_implement_backed_enum;

	memcpy(&enum_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	enum_handlers.read_property = zend_enum_read_property;
	enum_handlers.write_property = zend_enum_write_property;
	enum_handlers.unset_property = zend_enum_unset_property;
	enum_handlers.get_property_ptr_ptr = zend_enum_get_property_ptr_ptr;
	enum_handlers.clone_obj = NULL;
	enum_handlers.compare = zend_objects_not_comparable;
}

void zend_enum_add_interfaces(zend_class_entry *ce)
{
	uint32_t num_interfaces_before = ce->num_interfaces;

	ce->num_interfaces++;
	if (ce->enum_backing_type != IS_UNDEF) {
		ce->num_interfaces++;
	}

	ZEND_ASSERT(!(ce->ce_flags & ZEND_ACC_RESOLVED_INTERFACES));

	ce->interface_names = erealloc(ce->interface_names, sizeof(zend_class_name) * ce->num_interfaces);

	ce->interface_names[num_interfaces_before].name = zend_string_copy(zend_ce_unit_enum->name);
	ce->interface_names[num_interfaces_before].lc_name = zend_string_init("unitenum", sizeof("unitenum") - 1, 0);

	if (ce->enum_backing_type != IS_UNDEF) {
		ce->interface_names[num_interfaces_before + 1].name = zend_string_copy(zend_ce_backed_enum->name);
		ce->interface_names[num_interfaces_before + 1].lc_name = zend_string_init("backedenum", sizeof("backedenum") - 1, 0);	
	}
}

static ZEND_NAMED_FUNCTION(zend_enum_cases_func)
{
	zend_class_entry *ce = execute_data->func->common.scope;
	zend_class_constant *c;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZEND_HASH_FOREACH_PTR(CE_CONSTANTS_TABLE(ce), c) {
		if (!(ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE)) {
			continue;
		}
		zval *zv = &c->value;
		if (Z_TYPE_P(zv) == IS_CONSTANT_AST) {
			if (zval_update_constant_ex(zv, c->ce) == FAILURE) {
				RETURN_THROWS();
			}
		}
		Z_ADDREF_P(zv);
		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), zv);
	} ZEND_HASH_FOREACH_END();
}

static void zend_enum_from_base(INTERNAL_FUNCTION_PARAMETERS, bool try)
{
	zend_class_entry *ce = execute_data->func->common.scope;
	zend_string *string_key;
	zend_long long_key;

	zval *case_name_zv;
	if (ce->enum_backing_type == IS_LONG) {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_LONG(long_key)
		ZEND_PARSE_PARAMETERS_END();

		case_name_zv = zend_hash_index_find(ce->backed_enum_table, long_key);
	} else {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(string_key)
		ZEND_PARSE_PARAMETERS_END();

		ZEND_ASSERT(ce->enum_backing_type == IS_STRING);
		case_name_zv = zend_hash_find(ce->backed_enum_table, string_key);
	}

	if (case_name_zv == NULL) {
		if (try) {
			RETURN_NULL();
		}

		if (ce->enum_backing_type == IS_LONG) {
			zend_value_error(ZEND_LONG_FMT " is not a valid backing value for enum \"%s\"", long_key, ZSTR_VAL(ce->name));
		} else {
			ZEND_ASSERT(ce->enum_backing_type == IS_STRING);
			zend_value_error("\"%s\" is not a valid backing value for enum \"%s\"", ZSTR_VAL(string_key), ZSTR_VAL(ce->name));
		}
		RETURN_THROWS();
	}

	// TODO: We might want to store pointers to constants in backed_enum_table instead of names,
	// to make this lookup more efficient.
	ZEND_ASSERT(Z_TYPE_P(case_name_zv) == IS_STRING);
	zend_class_constant *c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), Z_STR_P(case_name_zv));
	ZEND_ASSERT(c != NULL);
	zval *case_zv = &c->value;
	if (Z_TYPE_P(case_zv) == IS_CONSTANT_AST) {
		if (zval_update_constant_ex(case_zv, c->ce) == FAILURE) {
			RETURN_THROWS();
		}
	}

	ZVAL_COPY(return_value, case_zv);
}

static ZEND_NAMED_FUNCTION(zend_enum_from_func)
{
	zend_enum_from_base(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

static ZEND_NAMED_FUNCTION(zend_enum_try_from_func)
{
	zend_enum_from_base(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

void zend_enum_register_funcs(zend_class_entry *ce)
{
	const uint32_t fn_flags =
		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_ARENA_ALLOCATED;
	zend_internal_function *cases_function =
		zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
	memset(cases_function, 0, sizeof(zend_internal_function));
	cases_function->type = ZEND_INTERNAL_FUNCTION;
	cases_function->module = EG(current_module);
	cases_function->handler = zend_enum_cases_func;
	cases_function->function_name = ZSTR_KNOWN(ZEND_STR_CASES);
	cases_function->scope = ce;
	cases_function->fn_flags = fn_flags;
	cases_function->arg_info = (zend_internal_arg_info *) (arginfo_class_UnitEnum_cases + 1);
	if (!zend_hash_add_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_CASES), cases_function)) {
		zend_error_noreturn(E_COMPILE_ERROR, "Cannot redeclare %s::cases()", ZSTR_VAL(ce->name));
	}

	if (ce->enum_backing_type != IS_UNDEF) {
		zend_internal_function *from_function =
			zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
		memset(from_function, 0, sizeof(zend_internal_function));
		from_function->type = ZEND_INTERNAL_FUNCTION;
		from_function->module = EG(current_module);
		from_function->handler = zend_enum_from_func;
		from_function->function_name = ZSTR_KNOWN(ZEND_STR_FROM);
		from_function->scope = ce;
		from_function->fn_flags = fn_flags;
		from_function->num_args = 1;
		from_function->required_num_args = 1;
		from_function->arg_info = (zend_internal_arg_info *) (arginfo_class_BackedEnum_from + 1);
		if (!zend_hash_add_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_FROM), from_function)) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot redeclare %s::from()", ZSTR_VAL(ce->name));
		}

		zend_internal_function *try_from_function =
			zend_arena_alloc(&CG(arena), sizeof(zend_internal_function));
		memset(try_from_function, 0, sizeof(zend_internal_function));
		try_from_function->type = ZEND_INTERNAL_FUNCTION;
		try_from_function->module = EG(current_module);
		try_from_function->handler = zend_enum_try_from_func;
		try_from_function->function_name = ZSTR_KNOWN(ZEND_STR_TRYFROM);
		try_from_function->scope = ce;
		try_from_function->fn_flags = fn_flags;
		try_from_function->num_args = 1;
		try_from_function->required_num_args = 1;
		try_from_function->arg_info = (zend_internal_arg_info *) (arginfo_class_BackedEnum_tryFrom + 1);
		if (!zend_hash_add_ptr(
				&ce->function_table, ZSTR_KNOWN(ZEND_STR_TRYFROM_LOWERCASE), try_from_function)) {
			zend_error_noreturn(E_COMPILE_ERROR,
				"Cannot redeclare %s::tryFrom()", ZSTR_VAL(ce->name));
		}
	}
}

void zend_enum_register_props(zend_class_entry *ce)
{
	zval name_default_value;
	ZVAL_UNDEF(&name_default_value);
	zend_type name_type = ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0);
	zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_NAME), &name_default_value, ZEND_ACC_PUBLIC, NULL, name_type);

	if (ce->enum_backing_type != IS_UNDEF) {
		zval value_default_value;
		ZVAL_UNDEF(&value_default_value);
		zend_type value_type = ZEND_TYPE_INIT_CODE(ce->enum_backing_type, 0, 0);
		zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_VALUE), &value_default_value, ZEND_ACC_PUBLIC, NULL, value_type);
	}
}

static const zend_function_entry simple_internal_methods[] = {
	ZEND_NAMED_ME(cases, zend_enum_cases_func, arginfo_class_UnitEnum_cases, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry backed_internal_methods[] = {
	ZEND_NAMED_ME(from, zend_enum_from_func, arginfo_class_BackedEnum_from, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_NAMED_ME(tryFrom, zend_enum_try_from_func, arginfo_class_BackedEnum_tryFrom, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

ZEND_API zend_class_entry *zend_register_internal_enum(const char *name)
{
	return zend_register_internal_backed_enum_ex(name, IS_UNDEF, NULL);
}

ZEND_API zend_class_entry *zend_register_internal_enum_ex(const char *name, const zend_function_entry *functions)
{
	return zend_register_internal_backed_enum_ex(name, IS_UNDEF, functions);
}

ZEND_API zend_class_entry *zend_register_internal_backed_enum(const char *name, uint32_t backing_type)
{
	return zend_register_internal_backed_enum_ex(name, backing_type, NULL);
}

ZEND_API zend_class_entry *zend_register_internal_backed_enum_ex(const char *name, uint32_t backing_type, const zend_function_entry *functions)
{
	ZEND_ASSERT(backing_type == IS_UNDEF || backing_type == IS_STRING || backing_type == IS_LONG);

	zend_class_entry ce_val, *ce;
	INIT_CLASS_ENTRY_EX(ce_val, name, strlen(name), functions);
	ce = zend_register_internal_class(&ce_val);

	ce->ce_flags |= ZEND_ACC_ENUM;
	ce->enum_backing_type = backing_type;
	ce->backed_enum_table = malloc(sizeof(HashTable));
	zend_hash_init(ce->backed_enum_table, 0, NULL, ZVAL_PTR_DTOR, 1);
	zend_enum_register_props(ce);

	zend_register_functions(ce, simple_internal_methods, &ce->function_table, EG(current_module)->type);
	zend_do_implement_interface(ce, zend_ce_unit_enum);
	if (backing_type != IS_UNDEF) {
		zend_register_functions(ce, backed_internal_methods, &ce->function_table, EG(current_module)->type);
		zend_do_implement_interface(ce, zend_ce_backed_enum);
	}

	return ce;
}

static zend_object *add_internal_enum_case(zend_class_entry *ce, zend_string *name, zval *value)
{
	zend_object *case_obj = zend_objects_new_persistent(ce);
	enum_obj_init(case_obj, name, value);

	zval casezv;
	ZVAL_OBJ(&casezv, case_obj);
	zend_class_constant *c = zend_declare_class_constant_ex(ce, name, &casezv, ZEND_ACC_PUBLIC, NULL);
	ZEND_CLASS_CONST_FLAGS(c) |= ZEND_CLASS_CONST_IS_CASE;

	zend_string_release(name);
	if (value) {
		zval_internal_ptr_dtor(value);
	}

	return case_obj;
}

ZEND_API zend_object *zend_add_enum_case_ex(zend_class_entry *ce, zend_string *name)
{
	ZEND_ASSERT(ce->enum_backing_type == IS_UNDEF);

	name = zend_new_interned_string(name);

	zend_object *case_obj = add_internal_enum_case(ce, name, NULL);

	zend_string_release(name);

	return case_obj;
}

ZEND_API zend_object *zend_add_enum_case_long_ex(zend_class_entry *ce, zend_string *name, zend_long value)
{
	ZEND_ASSERT(ce->enum_backing_type == IS_LONG);

	name = zend_new_interned_string(name);

	zval namezv;
	ZVAL_STR_COPY(&namezv, name);
	if (!zend_hash_index_add(ce->backed_enum_table, value, &namezv)) {
		ZEND_ASSERT(0 && "Cannot add two enum values with the same backing value");
	}

	zval zv;
	ZVAL_LONG(&zv, value);
	zend_object *case_obj = add_internal_enum_case(ce, name, &zv);

	zend_string_release(name);

	return case_obj;
}

ZEND_API zend_object *zend_add_enum_case_str_ex(zend_class_entry *ce, zend_string *name, zend_string *str)
{
	ZEND_ASSERT(ce->enum_backing_type == IS_STRING);

	name = zend_new_interned_string(name);
	str = zend_new_interned_string(str);

	zval namezv;
	ZVAL_STR_COPY(&namezv, name);
	if (!zend_hash_add(ce->backed_enum_table, str, &namezv)) {
		ZEND_ASSERT(0 && "Cannot add two enum values with the same backing value");
	}

	zval zv;
	ZVAL_STR(&zv, str);
	zend_object *case_obj = add_internal_enum_case(ce, name, &zv);

	zend_string_release(name);
	zend_string_release(str);

	return case_obj;
}
