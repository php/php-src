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
#include "zend_enum_arginfo.h"
#include "zend_interfaces.h"
#include "zend_enum.h"
#include "zend_extensions.h"
#include "zend_observer.h"

#define ZEND_ENUM_DISALLOW_MAGIC_METHOD(propertyName, methodName) \
	do { \
		if (ce->propertyName) { \
			zend_error_noreturn(E_COMPILE_ERROR, "Enum %s cannot include magic method %s", ZSTR_VAL(ce->name), methodName); \
		} \
	} while (0);

ZEND_API zend_class_entry *zend_ce_unit_enum;
ZEND_API zend_class_entry *zend_ce_backed_enum;
ZEND_API zend_object_handlers zend_enum_object_handlers;

zend_object *zend_enum_new(zval *result, zend_class_entry *ce, zend_string *case_name, zval *backing_value_zv)
{
	zend_object *zobj = zend_objects_new(ce);
	ZVAL_OBJ(result, zobj);

	zval *zname = OBJ_PROP_NUM(zobj, 0);
	ZVAL_STR_COPY(zname, case_name);
	/* ZVAL_COPY does not set Z_PROP_FLAG, this needs to be cleared to avoid leaving IS_PROP_REINITABLE set */
	Z_PROP_FLAG_P(zname) = 0;

	if (backing_value_zv != NULL) {
		zval *prop = OBJ_PROP_NUM(zobj, 1);

		ZVAL_COPY(prop, backing_value_zv);
		/* ZVAL_COPY does not set Z_PROP_FLAG, this needs to be cleared to avoid leaving IS_PROP_REINITABLE set */
		Z_PROP_FLAG_P(prop) = 0;
	}

	return zobj;
}

static void zend_verify_enum_properties(zend_class_entry *ce)
{
	zend_property_info *property_info;

	ZEND_HASH_MAP_FOREACH_PTR(&ce->properties_info, property_info) {
		if (zend_string_equals(property_info->name, ZSTR_KNOWN(ZEND_STR_NAME))) {
			continue;
		}
		if (
			ce->enum_backing_type != IS_UNDEF
			&& zend_string_equals(property_info->name, ZSTR_KNOWN(ZEND_STR_VALUE))
		) {
			continue;
		}
		// FIXME: File/line number for traits?
		zend_error_noreturn(E_COMPILE_ERROR, "Enum %s cannot include properties",
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

	static const char *const forbidden_methods[] = {
		"__sleep",
		"__wakeup",
		"__set_state",
	};

	uint32_t forbidden_methods_length = sizeof(forbidden_methods) / sizeof(forbidden_methods[0]);
	for (uint32_t i = 0; i < forbidden_methods_length; ++i) {
		const char *forbidden_method = forbidden_methods[i];

		if (zend_hash_str_exists(&ce->function_table, forbidden_method, strlen(forbidden_method))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Enum %s cannot include magic method %s", ZSTR_VAL(ce->name), forbidden_method);
		}
	}
}

static void zend_verify_enum_interfaces(zend_class_entry *ce)
{
	if (zend_class_implements_interface(ce, zend_ce_serializable)) {
		zend_error_noreturn(E_COMPILE_ERROR,
			"Enum %s cannot implement the Serializable interface", ZSTR_VAL(ce->name));
	}
}

void zend_verify_enum(zend_class_entry *ce)
{
	zend_verify_enum_properties(ce);
	zend_verify_enum_magic_methods(ce);
	zend_verify_enum_interfaces(ce);
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

	memcpy(&zend_enum_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	zend_enum_object_handlers.clone_obj = NULL;
	zend_enum_object_handlers.compare = zend_objects_not_comparable;
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
	ce->interface_names[num_interfaces_before].lc_name = ZSTR_INIT_LITERAL("unitenum", 0);

	if (ce->enum_backing_type != IS_UNDEF) {
		ce->interface_names[num_interfaces_before + 1].name = zend_string_copy(zend_ce_backed_enum->name);
		ce->interface_names[num_interfaces_before + 1].lc_name = ZSTR_INIT_LITERAL("backedenum", 0);
	}

	ce->default_object_handlers = &zend_enum_object_handlers;
}

zend_result zend_enum_build_backed_enum_table(zend_class_entry *ce)
{
	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_ENUM);
	ZEND_ASSERT(ce->type == ZEND_USER_CLASS);

	uint32_t backing_type = ce->enum_backing_type;
	ZEND_ASSERT(backing_type != IS_UNDEF);

	HashTable *backed_enum_table = emalloc(sizeof(HashTable));
	zend_hash_init(backed_enum_table, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_class_set_backed_enum_table(ce, backed_enum_table);

	zend_string *enum_class_name = ce->name;

	zend_string *name;
	zval *val;
	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(CE_CONSTANTS_TABLE(ce), name, val) {
		zend_class_constant *c = Z_PTR_P(val);
		if ((ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE) == 0) {
			continue;
		}

		zval *c_value = &c->value;
		zval *case_name = zend_enum_fetch_case_name(Z_OBJ_P(c_value));
		zval *case_value = zend_enum_fetch_case_value(Z_OBJ_P(c_value));

		if (ce->enum_backing_type != Z_TYPE_P(case_value)) {
			zend_type_error("Enum case type %s does not match enum backing type %s",
				zend_get_type_by_const(Z_TYPE_P(case_value)),
				zend_get_type_by_const(ce->enum_backing_type));
			goto failure;
		}

		if (ce->enum_backing_type == IS_LONG) {
			zend_long long_key = Z_LVAL_P(case_value);
			zval *existing_case_name = zend_hash_index_find(backed_enum_table, long_key);
			if (existing_case_name) {
				zend_throw_error(NULL, "Duplicate value in enum %s for cases %s and %s",
					ZSTR_VAL(enum_class_name),
					Z_STRVAL_P(existing_case_name),
					ZSTR_VAL(name));
				goto failure;
			}
			zend_hash_index_add_new(backed_enum_table, long_key, case_name);
		} else {
			ZEND_ASSERT(ce->enum_backing_type == IS_STRING);
			zend_string *string_key = Z_STR_P(case_value);
			zval *existing_case_name = zend_hash_find(backed_enum_table, string_key);
			if (existing_case_name != NULL) {
				zend_throw_error(NULL, "Duplicate value in enum %s for cases %s and %s",
					ZSTR_VAL(enum_class_name),
					Z_STRVAL_P(existing_case_name),
					ZSTR_VAL(name));
				goto failure;
			}
			zend_hash_add_new(backed_enum_table, string_key, case_name);
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;

failure:
	zend_hash_release(backed_enum_table);
	zend_class_set_backed_enum_table(ce, NULL);
	return FAILURE;
}

static ZEND_NAMED_FUNCTION(zend_enum_cases_func)
{
	zend_class_entry *ce = execute_data->func->common.scope;
	zend_class_constant *c;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZEND_HASH_MAP_FOREACH_PTR(CE_CONSTANTS_TABLE(ce), c) {
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

ZEND_API zend_result zend_enum_get_case_by_value(zend_object **result, zend_class_entry *ce, zend_long long_key, zend_string *string_key, bool try)
{
	if (ce->type == ZEND_USER_CLASS && !(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)) {
		if (zend_update_class_constants(ce) == FAILURE) {
			return FAILURE;
		}
	}

	HashTable *backed_enum_table = CE_BACKED_ENUM_TABLE(ce);
	if (!backed_enum_table) {
		goto not_found;
	}

	zval *case_name_zv;
	if (ce->enum_backing_type == IS_LONG) {
		case_name_zv = zend_hash_index_find(backed_enum_table, long_key);
	} else {
		ZEND_ASSERT(ce->enum_backing_type == IS_STRING);
		ZEND_ASSERT(string_key != NULL);
		case_name_zv = zend_hash_find(backed_enum_table, string_key);
	}

	if (case_name_zv == NULL) {
not_found:
		if (try) {
			*result = NULL;
			return SUCCESS;
		}

		if (ce->enum_backing_type == IS_LONG) {
			zend_value_error(ZEND_LONG_FMT " is not a valid backing value for enum %s", long_key, ZSTR_VAL(ce->name));
		} else {
			ZEND_ASSERT(ce->enum_backing_type == IS_STRING);
			zend_value_error("\"%s\" is not a valid backing value for enum %s", ZSTR_VAL(string_key), ZSTR_VAL(ce->name));
		}
		return FAILURE;
	}

	// TODO: We might want to store pointers to constants in backed_enum_table instead of names,
	// to make this lookup more efficient.
	ZEND_ASSERT(Z_TYPE_P(case_name_zv) == IS_STRING);
	zend_class_constant *c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), Z_STR_P(case_name_zv));
	ZEND_ASSERT(c != NULL);
	zval *case_zv = &c->value;
	if (Z_TYPE_P(case_zv) == IS_CONSTANT_AST) {
		if (zval_update_constant_ex(case_zv, c->ce) == FAILURE) {
			return FAILURE;
		}
	}

	*result = Z_OBJ_P(case_zv);
	return SUCCESS;
}

static void zend_enum_from_base(INTERNAL_FUNCTION_PARAMETERS, bool try)
{
	zend_class_entry *ce = execute_data->func->common.scope;
	bool release_string = false;
	zend_string *string_key = NULL;
	zend_long long_key = 0;

	if (ce->enum_backing_type == IS_LONG) {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_LONG(long_key)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		ZEND_ASSERT(ce->enum_backing_type == IS_STRING);

		if (ZEND_ARG_USES_STRICT_TYPES()) {
			ZEND_PARSE_PARAMETERS_START(1, 1)
				Z_PARAM_STR(string_key)
			ZEND_PARSE_PARAMETERS_END();
		} else {
			// We allow long keys so that coercion to string doesn't happen implicitly. The JIT
			// skips deallocation of params that don't require it. In the case of from/tryFrom
			// passing int to from(int|string) looks like no coercion will happen, so the JIT
			// won't emit a dtor call. Thus we allocate/free the string manually.
			ZEND_PARSE_PARAMETERS_START(1, 1)
				Z_PARAM_STR_OR_LONG(string_key, long_key)
			ZEND_PARSE_PARAMETERS_END();

			if (string_key == NULL) {
				release_string = true;
				string_key = zend_long_to_str(long_key);
			}
		}
	}

	zend_object *case_obj;
	if (zend_enum_get_case_by_value(&case_obj, ce, long_key, string_key, try) == FAILURE) {
		goto throw;
	}

	if (case_obj == NULL) {
		ZEND_ASSERT(try);
		goto return_null;
	}

	if (release_string) {
		zend_string_release(string_key);
	}
	RETURN_OBJ_COPY(case_obj);

throw:
	if (release_string) {
		zend_string_release(string_key);
	}
	RETURN_THROWS();

return_null:
	if (release_string) {
		zend_string_release(string_key);
	}
	RETURN_NULL();
}

static ZEND_NAMED_FUNCTION(zend_enum_from_func)
{
	zend_enum_from_base(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

static ZEND_NAMED_FUNCTION(zend_enum_try_from_func)
{
	zend_enum_from_base(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

static void zend_enum_register_func(zend_class_entry *ce, zend_known_string_id name_id, zend_internal_function *zif) {
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

void zend_enum_register_funcs(zend_class_entry *ce)
{
	const uint32_t fn_flags =
		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC|ZEND_ACC_HAS_RETURN_TYPE|ZEND_ACC_ARENA_ALLOCATED;
	zend_internal_function *cases_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
	cases_function->handler = zend_enum_cases_func;
	cases_function->function_name = ZSTR_KNOWN(ZEND_STR_CASES);
	cases_function->fn_flags = fn_flags;
	cases_function->arg_info = (zend_internal_arg_info *) (arginfo_class_UnitEnum_cases + 1);
	zend_enum_register_func(ce, ZEND_STR_CASES, cases_function);

	if (ce->enum_backing_type != IS_UNDEF) {
		zend_internal_function *from_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
		from_function->handler = zend_enum_from_func;
		from_function->function_name = ZSTR_KNOWN(ZEND_STR_FROM);
		from_function->fn_flags = fn_flags;
		from_function->num_args = 1;
		from_function->required_num_args = 1;
		from_function->arg_info = (zend_internal_arg_info *) (arginfo_class_BackedEnum_from + 1);
		zend_enum_register_func(ce, ZEND_STR_FROM, from_function);

		zend_internal_function *try_from_function = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
		try_from_function->handler = zend_enum_try_from_func;
		try_from_function->function_name = ZSTR_KNOWN(ZEND_STR_TRYFROM);
		try_from_function->fn_flags = fn_flags;
		try_from_function->num_args = 1;
		try_from_function->required_num_args = 1;
		try_from_function->arg_info = (zend_internal_arg_info *) (arginfo_class_BackedEnum_tryFrom + 1);
		zend_enum_register_func(ce, ZEND_STR_TRYFROM_LOWERCASE, try_from_function);
	}
}

void zend_enum_register_props(zend_class_entry *ce)
{
	ce->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

	zval name_default_value;
	ZVAL_UNDEF(&name_default_value);
	zend_type name_type = ZEND_TYPE_INIT_CODE(IS_STRING, 0, 0);
	zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_NAME), &name_default_value, ZEND_ACC_PUBLIC | ZEND_ACC_READONLY, NULL, name_type);

	if (ce->enum_backing_type != IS_UNDEF) {
		zval value_default_value;
		ZVAL_UNDEF(&value_default_value);
		zend_type value_type = ZEND_TYPE_INIT_CODE(ce->enum_backing_type, 0, 0);
		zend_declare_typed_property(ce, ZSTR_KNOWN(ZEND_STR_VALUE), &value_default_value, ZEND_ACC_PUBLIC | ZEND_ACC_READONLY, NULL, value_type);
	}
}

static const zend_function_entry unit_enum_methods[] = {
	ZEND_NAMED_ME(cases, zend_enum_cases_func, arginfo_class_UnitEnum_cases, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

static const zend_function_entry backed_enum_methods[] = {
	ZEND_NAMED_ME(cases, zend_enum_cases_func, arginfo_class_UnitEnum_cases, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_NAMED_ME(from, zend_enum_from_func, arginfo_class_BackedEnum_from, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_NAMED_ME(tryFrom, zend_enum_try_from_func, arginfo_class_BackedEnum_tryFrom, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

ZEND_API zend_class_entry *zend_register_internal_enum(
	const char *name, uint8_t type, const zend_function_entry *functions)
{
	ZEND_ASSERT(type == IS_UNDEF || type == IS_LONG || type == IS_STRING);

	zend_class_entry tmp_ce;
	INIT_CLASS_ENTRY_EX(tmp_ce, name, strlen(name), functions);

	zend_class_entry *ce = zend_register_internal_class(&tmp_ce);
	ce->ce_flags |= ZEND_ACC_ENUM;
	ce->enum_backing_type = type;
	if (type != IS_UNDEF) {
		HashTable *backed_enum_table = pemalloc(sizeof(HashTable), 1);
		zend_hash_init(backed_enum_table, 0, NULL, ZVAL_PTR_DTOR, 1);
		zend_class_set_backed_enum_table(ce, backed_enum_table);
	}

	zend_enum_register_props(ce);
	if (type == IS_UNDEF) {
		zend_register_functions(
			ce, unit_enum_methods, &ce->function_table, EG(current_module)->type);
		zend_class_implements(ce, 1, zend_ce_unit_enum);
	} else {
		zend_register_functions(
			ce, backed_enum_methods, &ce->function_table, EG(current_module)->type);
		zend_class_implements(ce, 1, zend_ce_backed_enum);
	}

	return ce;
}

static zend_ast_ref *create_enum_case_ast(
		zend_string *class_name, zend_string *case_name, zval *value) {
	// TODO: Use custom node type for enum cases?
	size_t size = sizeof(zend_ast_ref) + zend_ast_size(3)
		+ (value ? 3 : 2) * sizeof(zend_ast_zval);
	char *p = pemalloc(size, 1);
	zend_ast_ref *ref = (zend_ast_ref *) p; p += sizeof(zend_ast_ref);
	GC_SET_REFCOUNT(ref, 1);
	GC_TYPE_INFO(ref) = GC_CONSTANT_AST | GC_PERSISTENT | GC_IMMUTABLE;

	zend_ast *ast = (zend_ast *) p; p += zend_ast_size(3);
	ast->kind = ZEND_AST_CONST_ENUM_INIT;
	ast->attr = 0;
	ast->lineno = 0;

	ast->child[0] = (zend_ast *) p; p += sizeof(zend_ast_zval);
	ast->child[0]->kind = ZEND_AST_ZVAL;
	ast->child[0]->attr = 0;
	ZEND_ASSERT(ZSTR_IS_INTERNED(class_name));
	ZVAL_STR(zend_ast_get_zval(ast->child[0]), class_name);

	ast->child[1] = (zend_ast *) p; p += sizeof(zend_ast_zval);
	ast->child[1]->kind = ZEND_AST_ZVAL;
	ast->child[1]->attr = 0;
	ZEND_ASSERT(ZSTR_IS_INTERNED(case_name));
	ZVAL_STR(zend_ast_get_zval(ast->child[1]), case_name);

	if (value) {
		ast->child[2] = (zend_ast *) p; p += sizeof(zend_ast_zval);
		ast->child[2]->kind = ZEND_AST_ZVAL;
		ast->child[2]->attr = 0;
		ZEND_ASSERT(!Z_REFCOUNTED_P(value));
		ZVAL_COPY_VALUE(zend_ast_get_zval(ast->child[2]), value);
	} else {
		ast->child[2] = NULL;
	}

	return ref;
}

ZEND_API void zend_enum_add_case(zend_class_entry *ce, zend_string *case_name, zval *value)
{
	if (value) {
		ZEND_ASSERT(ce->enum_backing_type == Z_TYPE_P(value));
		if (Z_TYPE_P(value) == IS_STRING && !ZSTR_IS_INTERNED(Z_STR_P(value))) {
			zval_make_interned_string(value);
		}

		HashTable *backed_enum_table = CE_BACKED_ENUM_TABLE(ce);

		zval case_name_zv;
		ZVAL_STR(&case_name_zv, case_name);
		if (Z_TYPE_P(value) == IS_LONG) {
			zend_hash_index_add_new(backed_enum_table, Z_LVAL_P(value), &case_name_zv);
		} else {
			zend_hash_add_new(backed_enum_table, Z_STR_P(value), &case_name_zv);
		}
	} else {
		ZEND_ASSERT(ce->enum_backing_type == IS_UNDEF);
	}

	zval ast_zv;
	Z_TYPE_INFO(ast_zv) = IS_CONSTANT_AST;
	Z_AST(ast_zv) = create_enum_case_ast(ce->name, case_name, value);
	zend_class_constant *c = zend_declare_class_constant_ex(
		ce, case_name, &ast_zv, ZEND_ACC_PUBLIC, NULL);
	ZEND_CLASS_CONST_FLAGS(c) |= ZEND_CLASS_CONST_IS_CASE;
}

ZEND_API void zend_enum_add_case_cstr(zend_class_entry *ce, const char *name, zval *value)
{
	zend_string *name_str = zend_string_init_interned(name, strlen(name), 1);
	zend_enum_add_case(ce, name_str, value);
	zend_string_release(name_str);
}

ZEND_API zend_object *zend_enum_get_case(zend_class_entry *ce, zend_string *name) {
	zend_class_constant *c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), name);
	ZEND_ASSERT(c && "Must be a valid enum case");
	ZEND_ASSERT(ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE);

	if (Z_TYPE(c->value) == IS_CONSTANT_AST) {
		if (zval_update_constant_ex(&c->value, c->ce) == FAILURE) {
			ZEND_UNREACHABLE();
		}
	}
	ZEND_ASSERT(Z_TYPE(c->value) == IS_OBJECT);
	return Z_OBJ(c->value);
}

ZEND_API zend_object *zend_enum_get_case_cstr(zend_class_entry *ce, const char *name) {
	zend_string *name_str = zend_string_init(name, strlen(name), 0);
	zend_object *result = zend_enum_get_case(ce, name_str);
	zend_string_release(name_str);
	return result;
}
