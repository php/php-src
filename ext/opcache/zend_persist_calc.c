/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "ZendAccelerator.h"
#include "zend_persist.h"
#include "zend_extensions.h"
#include "zend_shared_alloc.h"
#include "zend_operators.h"
#include "zend_attributes.h"

#define ADD_DUP_SIZE(m,s)  ZCG(current_persistent_script)->size += zend_shared_memdup_size((void*)m, s)
#define ADD_SIZE(m)        ZCG(current_persistent_script)->size += ZEND_ALIGNED_SIZE(m)
#define ADD_ARENA_SIZE(m)  ZCG(current_persistent_script)->arena_size += ZEND_ALIGNED_SIZE(m)

#define ADD_SIZE_EX(m) do { \
		if (ZCG(is_immutable_class)) { \
			ADD_SIZE(m); \
		} else { \
			ADD_ARENA_SIZE(m); \
		} \
	} while (0)

# define ADD_STRING(str) ADD_DUP_SIZE((str), _ZSTR_STRUCT_SIZE(ZSTR_LEN(str)))

# define ADD_INTERNED_STRING(str) do { \
		if (ZCG(current_persistent_script)->corrupted) { \
			ADD_STRING(str); \
		} else if (!IS_ACCEL_INTERNED(str)) { \
			zend_string *tmp = accel_new_interned_string(str); \
			if (tmp != (str)) { \
				(str) = tmp; \
			} else { \
				ADD_STRING(str); \
			} \
		} \
	} while (0)

static void zend_persist_zval_calc(zval *z);

static void zend_hash_persist_calc(HashTable *ht)
{
	if ((HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) || ht->nNumUsed == 0) {
		return;
	}

	if (!(HT_FLAGS(ht) & HASH_FLAG_PACKED) && ht->nNumUsed > HT_MIN_SIZE && ht->nNumUsed < (uint32_t)(-(int32_t)ht->nTableMask) / 4) {
		/* compact table */
		uint32_t hash_size;

		hash_size = (uint32_t)(-(int32_t)ht->nTableMask);
		while (hash_size >> 2 > ht->nNumUsed) {
			hash_size >>= 1;
		}
		ADD_SIZE(hash_size * sizeof(uint32_t) + ht->nNumUsed * sizeof(Bucket));
	} else {
		ADD_SIZE(HT_USED_SIZE(ht));
	}
}

static void zend_persist_ast_calc(zend_ast *ast)
{
	uint32_t i;

	if (ast->kind == ZEND_AST_ZVAL || ast->kind == ZEND_AST_CONSTANT) {
		ADD_SIZE(sizeof(zend_ast_zval));
		zend_persist_zval_calc(&((zend_ast_zval*)(ast))->val);
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		ADD_SIZE(sizeof(zend_ast_list) - sizeof(zend_ast *) + sizeof(zend_ast *) * list->children);
		for (i = 0; i < list->children; i++) {
			if (list->child[i]) {
				zend_persist_ast_calc(list->child[i]);
			}
		}
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		ADD_SIZE(sizeof(zend_ast) - sizeof(zend_ast *) + sizeof(zend_ast *) * children);
		for (i = 0; i < children; i++) {
			if (ast->child[i]) {
				zend_persist_ast_calc(ast->child[i]);
			}
		}
	}
}

static void zend_persist_zval_calc(zval *z)
{
	uint32_t size;

	switch (Z_TYPE_P(z)) {
		case IS_STRING:
			ADD_INTERNED_STRING(Z_STR_P(z));
			if (ZSTR_IS_INTERNED(Z_STR_P(z))) {
				Z_TYPE_FLAGS_P(z) = 0;
			}
			break;
		case IS_ARRAY:
			size = zend_shared_memdup_size(Z_ARR_P(z), sizeof(zend_array));
			if (size) {
				Bucket *p;

				ADD_SIZE(size);
				zend_hash_persist_calc(Z_ARRVAL_P(z));
				ZEND_HASH_FOREACH_BUCKET(Z_ARRVAL_P(z), p) {
					if (p->key) {
						ADD_INTERNED_STRING(p->key);
					}
					zend_persist_zval_calc(&p->val);
				} ZEND_HASH_FOREACH_END();
			}
			break;
		case IS_CONSTANT_AST:
			size = zend_shared_memdup_size(Z_AST_P(z), sizeof(zend_ast_ref));
			if (size) {
				ADD_SIZE(size);
				zend_persist_ast_calc(Z_ASTVAL_P(z));
			}
			break;
		default:
			ZEND_ASSERT(Z_TYPE_P(z) < IS_STRING);
			break;
	}
}

static void zend_persist_attributes_calc(HashTable *attributes)
{
	if (!zend_shared_alloc_get_xlat_entry(attributes)) {
		zend_attribute *attr;
		uint32_t i;

		zend_shared_alloc_register_xlat_entry(attributes, attributes);
		ADD_SIZE(sizeof(HashTable));
		zend_hash_persist_calc(attributes);

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			ADD_SIZE(ZEND_ATTRIBUTE_SIZE(attr->argc));
			ADD_INTERNED_STRING(attr->name);
			ADD_INTERNED_STRING(attr->lcname);

			for (i = 0; i < attr->argc; i++) {
				if (attr->args[i].name) {
					ADD_INTERNED_STRING(attr->args[i].name);
				}
				zend_persist_zval_calc(&attr->args[i].value);
			}
		} ZEND_HASH_FOREACH_END();
	}
}

static void zend_persist_type_calc(zend_type *type)
{
	if (ZEND_TYPE_HAS_LIST(*type)) {
		if (ZEND_TYPE_USES_ARENA(*type) && !ZCG(is_immutable_class)) {
			ADD_ARENA_SIZE(ZEND_TYPE_LIST_SIZE(ZEND_TYPE_LIST(*type)->num_types));
		} else {
			ADD_SIZE(ZEND_TYPE_LIST_SIZE(ZEND_TYPE_LIST(*type)->num_types));
		}
	}

	zend_type *single_type;
	ZEND_TYPE_FOREACH(*type, single_type) {
		if (ZEND_TYPE_HAS_NAME(*single_type)) {
			zend_string *type_name = ZEND_TYPE_NAME(*single_type);
			ADD_INTERNED_STRING(type_name);
			ZEND_TYPE_SET_PTR(*single_type, type_name);
		}
	} ZEND_TYPE_FOREACH_END();
}

static void zend_persist_op_array_calc_ex(zend_op_array *op_array)
{
	if (op_array->function_name) {
		zend_string *old_name = op_array->function_name;
		ADD_INTERNED_STRING(op_array->function_name);
		/* Remember old function name, so it can be released multiple times if shared. */
		if (op_array->function_name != old_name
				&& !zend_shared_alloc_get_xlat_entry(&op_array->function_name)) {
			zend_shared_alloc_register_xlat_entry(&op_array->function_name, old_name);
		}
    }

	if (op_array->scope && zend_shared_alloc_get_xlat_entry(op_array->opcodes)) {
		/* already stored */
		ADD_SIZE(ZEND_ALIGNED_SIZE(zend_extensions_op_array_persist_calc(op_array)));
		return;
	}

	if (op_array->static_variables) {
		if (!zend_shared_alloc_get_xlat_entry(op_array->static_variables)) {
			Bucket *p;

			zend_shared_alloc_register_xlat_entry(op_array->static_variables, op_array->static_variables);
			ADD_SIZE(sizeof(HashTable));
			zend_hash_persist_calc(op_array->static_variables);
			ZEND_HASH_FOREACH_BUCKET(op_array->static_variables, p) {
				ZEND_ASSERT(p->key != NULL);
				ADD_INTERNED_STRING(p->key);
				zend_persist_zval_calc(&p->val);
			} ZEND_HASH_FOREACH_END();
		}
	}

	if (op_array->literals) {
		zval *p = op_array->literals;
		zval *end = p + op_array->last_literal;
		ADD_SIZE(sizeof(zval) * op_array->last_literal);
		while (p < end) {
			zend_persist_zval_calc(p);
			p++;
		}
	}

	zend_shared_alloc_register_xlat_entry(op_array->opcodes, op_array->opcodes);
	ADD_SIZE(sizeof(zend_op) * op_array->last);

	if (op_array->filename) {
		ADD_STRING(op_array->filename);
	}

	if (op_array->arg_info) {
		zend_arg_info *arg_info = op_array->arg_info;
		uint32_t num_args = op_array->num_args;
		uint32_t i;

		num_args = op_array->num_args;
		if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			arg_info--;
			num_args++;
		}
		ADD_SIZE(sizeof(zend_arg_info) * num_args);
		for (i = 0; i < num_args; i++) {
			if (arg_info[i].name) {
				ADD_INTERNED_STRING(arg_info[i].name);
			}
			zend_persist_type_calc(&arg_info[i].type);
		}
	}

	if (op_array->live_range) {
		ADD_SIZE(sizeof(zend_live_range) * op_array->last_live_range);
	}

	if (ZCG(accel_directives).save_comments && op_array->doc_comment) {
		ADD_STRING(op_array->doc_comment);
	}

	if (op_array->attributes) {
		zend_persist_attributes_calc(op_array->attributes);
	}

	if (op_array->try_catch_array) {
		ADD_SIZE(sizeof(zend_try_catch_element) * op_array->last_try_catch);
	}

	if (op_array->vars) {
		int i;

		ADD_SIZE(sizeof(zend_string*) * op_array->last_var);
		for (i = 0; i < op_array->last_var; i++) {
			ADD_INTERNED_STRING(op_array->vars[i]);
		}
	}

	ADD_SIZE(ZEND_ALIGNED_SIZE(zend_extensions_op_array_persist_calc(op_array)));
}

static void zend_persist_op_array_calc(zval *zv)
{
	zend_op_array *op_array = Z_PTR_P(zv);

	ZEND_ASSERT(op_array->type == ZEND_USER_FUNCTION);
	ADD_SIZE(sizeof(zend_op_array));
	zend_persist_op_array_calc_ex(Z_PTR_P(zv));
	if (ZCG(current_persistent_script)->corrupted) {
		ADD_ARENA_SIZE(sizeof(void*));
	}
}

static void zend_persist_class_method_calc(zval *zv)
{
	zend_op_array *op_array = Z_PTR_P(zv);
	zend_op_array *old_op_array;

	if (op_array->type != ZEND_USER_FUNCTION) {
		ZEND_ASSERT(op_array->type == ZEND_INTERNAL_FUNCTION);
		if (op_array->fn_flags & ZEND_ACC_ARENA_ALLOCATED) {
			old_op_array = zend_shared_alloc_get_xlat_entry(op_array);
			if (!old_op_array) {
				ADD_SIZE_EX(sizeof(zend_internal_function));
				zend_shared_alloc_register_xlat_entry(op_array, Z_PTR_P(zv));
			}
		}
		return;
	}

	old_op_array = zend_shared_alloc_get_xlat_entry(op_array);
	if (!old_op_array) {
		ADD_SIZE_EX(sizeof(zend_op_array));
		zend_persist_op_array_calc_ex(Z_PTR_P(zv));
		zend_shared_alloc_register_xlat_entry(op_array, Z_PTR_P(zv));
		if (!ZCG(is_immutable_class)) {
			ADD_ARENA_SIZE(sizeof(void*));
		}
	} else {
		/* If op_array is shared, the function name refcount is still incremented for each use,
		 * so we need to release it here. We remembered the original function name in xlat. */
		zend_string *old_function_name =
			zend_shared_alloc_get_xlat_entry(&old_op_array->function_name);
		if (old_function_name) {
			zend_string_release_ex(old_function_name, 0);
		}
	}
}

static void zend_persist_property_info_calc(zend_property_info *prop)
{
	ADD_SIZE_EX(sizeof(zend_property_info));
	ADD_INTERNED_STRING(prop->name);
	zend_persist_type_calc(&prop->type);
	if (ZCG(accel_directives).save_comments && prop->doc_comment) {
		ADD_STRING(prop->doc_comment);
	}
	if (prop->attributes) {
		zend_persist_attributes_calc(prop->attributes);
	}
}

static void zend_persist_class_constant_calc(zval *zv)
{
	zend_class_constant *c = Z_PTR_P(zv);

	if (!zend_shared_alloc_get_xlat_entry(c)) {
		zend_shared_alloc_register_xlat_entry(c, c);
		ADD_SIZE_EX(sizeof(zend_class_constant));
		zend_persist_zval_calc(&c->value);
		if (ZCG(accel_directives).save_comments && c->doc_comment) {
			ADD_STRING(c->doc_comment);
		}
		if (c->attributes) {
			zend_persist_attributes_calc(c->attributes);
		}
	}
}

static void check_property_type_resolution(zend_class_entry *ce) {
	zend_property_info *prop;
	if (ce->ce_flags & ZEND_ACC_PROPERTY_TYPES_RESOLVED) {
		/* Preloading might have computed this already. */
		return;
	}

	if (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS) {
		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			zend_type *single_type;
			ZEND_TYPE_FOREACH(prop->type, single_type) {
				if (ZEND_TYPE_HAS_NAME(*single_type)) {
					return;
				}
			} ZEND_TYPE_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}
	ce->ce_flags |= ZEND_ACC_PROPERTY_TYPES_RESOLVED;
}

static void zend_persist_class_entry_calc(zval *zv)
{
	zend_class_entry *ce = Z_PTR_P(zv);
	Bucket *p;

	if (ce->type == ZEND_USER_CLASS) {
		/* The same zend_class_entry may be reused by class_alias */
		if (zend_shared_alloc_get_xlat_entry(ce)) {
			return;
		}
		zend_shared_alloc_register_xlat_entry(ce, ce);

		check_property_type_resolution(ce);

		ZCG(is_immutable_class) =
			(ce->ce_flags & ZEND_ACC_LINKED) &&
			(ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED) &&
			(ce->ce_flags & ZEND_ACC_PROPERTY_TYPES_RESOLVED) &&
			!ZCG(current_persistent_script)->corrupted;

		ADD_SIZE_EX(sizeof(zend_class_entry));
		ADD_INTERNED_STRING(ce->name);
		if (ce->parent_name && !(ce->ce_flags & ZEND_ACC_LINKED)) {
			ADD_INTERNED_STRING(ce->parent_name);
		}
		zend_hash_persist_calc(&ce->function_table);
		ZEND_HASH_FOREACH_BUCKET(&ce->function_table, p) {
			ZEND_ASSERT(p->key != NULL);
			ADD_INTERNED_STRING(p->key);
			zend_persist_class_method_calc(&p->val);
		} ZEND_HASH_FOREACH_END();
		if (ce->default_properties_table) {
		    int i;

			ADD_SIZE(sizeof(zval) * ce->default_properties_count);
			for (i = 0; i < ce->default_properties_count; i++) {
				zend_persist_zval_calc(&ce->default_properties_table[i]);
			}
		}
		if (ce->default_static_members_table) {
		    int i;

			ADD_SIZE(sizeof(zval) * ce->default_static_members_count);
			for (i = 0; i < ce->default_static_members_count; i++) {
				if (Z_TYPE(ce->default_static_members_table[i]) != IS_INDIRECT) {
					zend_persist_zval_calc(&ce->default_static_members_table[i]);
				}
			}
		}
		zend_hash_persist_calc(&ce->constants_table);
		ZEND_HASH_FOREACH_BUCKET(&ce->constants_table, p) {
			ZEND_ASSERT(p->key != NULL);
			ADD_INTERNED_STRING(p->key);
			zend_persist_class_constant_calc(&p->val);
		} ZEND_HASH_FOREACH_END();

		if (ce->info.user.filename) {
			ADD_STRING(ce->info.user.filename);
		}
		if (ZCG(accel_directives).save_comments && ce->info.user.doc_comment) {
			ADD_STRING(ce->info.user.doc_comment);
		}
		if (ce->attributes) {
			zend_persist_attributes_calc(ce->attributes);
		}

		zend_hash_persist_calc(&ce->properties_info);
		ZEND_HASH_FOREACH_BUCKET(&ce->properties_info, p) {
			zend_property_info *prop = Z_PTR(p->val);
			ZEND_ASSERT(p->key != NULL);
			ADD_INTERNED_STRING(p->key);
			if (prop->ce == ce) {
				zend_persist_property_info_calc(prop);
			}
		} ZEND_HASH_FOREACH_END();

		if (ce->properties_info_table) {
			ADD_SIZE_EX(sizeof(zend_property_info *) * ce->default_properties_count);
		}

		if (ce->num_interfaces) {
			uint32_t i;

			if (!(ce->ce_flags & ZEND_ACC_LINKED)) {
				for (i = 0; i < ce->num_interfaces; i++) {
					ADD_INTERNED_STRING(ce->interface_names[i].name);
					ADD_INTERNED_STRING(ce->interface_names[i].lc_name);
				}
				ADD_SIZE(sizeof(zend_class_name) * ce->num_interfaces);
			} else {
				ADD_SIZE(sizeof(zend_class_entry*) * ce->num_interfaces);
			}
		}

		if (ce->num_traits) {
			uint32_t i;

			for (i = 0; i < ce->num_traits; i++) {
				ADD_INTERNED_STRING(ce->trait_names[i].name);
				ADD_INTERNED_STRING(ce->trait_names[i].lc_name);
			}
			ADD_SIZE(sizeof(zend_class_name) * ce->num_traits);

			if (ce->trait_aliases) {
				i = 0;
				while (ce->trait_aliases[i]) {
					if (ce->trait_aliases[i]->trait_method.method_name) {
						ADD_INTERNED_STRING(ce->trait_aliases[i]->trait_method.method_name);
					}
					if (ce->trait_aliases[i]->trait_method.class_name) {
						ADD_INTERNED_STRING(ce->trait_aliases[i]->trait_method.class_name);
					}

					if (ce->trait_aliases[i]->alias) {
						ADD_INTERNED_STRING(ce->trait_aliases[i]->alias);
					}
					ADD_SIZE(sizeof(zend_trait_alias));
					i++;
				}
				ADD_SIZE(sizeof(zend_trait_alias*) * (i + 1));
			}

			if (ce->trait_precedences) {
				int j;

				i = 0;
				while (ce->trait_precedences[i]) {
					ADD_INTERNED_STRING(ce->trait_precedences[i]->trait_method.method_name);
					ADD_INTERNED_STRING(ce->trait_precedences[i]->trait_method.class_name);

					for (j = 0; j < ce->trait_precedences[i]->num_excludes; j++) {
						ADD_INTERNED_STRING(ce->trait_precedences[i]->exclude_class_names[j]);
					}
					ADD_SIZE(sizeof(zend_trait_precedence) + (ce->trait_precedences[i]->num_excludes - 1) * sizeof(zend_string*));
					i++;
				}
				ADD_SIZE(sizeof(zend_trait_precedence*) * (i + 1));
			}
		}

		if (ce->iterator_funcs_ptr) {
			ADD_SIZE(sizeof(zend_class_iterator_funcs));
		}
	}
}

static void zend_accel_persist_class_table_calc(HashTable *class_table)
{
	Bucket *p;

	zend_hash_persist_calc(class_table);
	ZEND_HASH_FOREACH_BUCKET(class_table, p) {
		ZEND_ASSERT(p->key != NULL);
		ADD_INTERNED_STRING(p->key);
		zend_persist_class_entry_calc(&p->val);
	} ZEND_HASH_FOREACH_END();
}

static void zend_persist_warnings_calc(zend_persistent_script *script) {
	ADD_SIZE(script->num_warnings * sizeof(zend_recorded_warning *));
	for (uint32_t i = 0; i < script->num_warnings; i++) {
		ADD_SIZE(sizeof(zend_recorded_warning));
		ADD_STRING(script->warnings[i]->error_filename);
		ADD_STRING(script->warnings[i]->error_message);
	}
}

uint32_t zend_accel_script_persist_calc(zend_persistent_script *new_persistent_script, const char *key, unsigned int key_length, int for_shm)
{
	Bucket *p;

	new_persistent_script->mem = NULL;
	new_persistent_script->size = 0;
	new_persistent_script->arena_mem = NULL;
	new_persistent_script->arena_size = 0;
	new_persistent_script->corrupted = 0;
	ZCG(current_persistent_script) = new_persistent_script;

	if (!for_shm) {
		/* script is not going to be saved in SHM */
		new_persistent_script->corrupted = 1;
	}

	ADD_SIZE(sizeof(zend_persistent_script));
	if (key) {
		ADD_SIZE(key_length + 1);
		zend_shared_alloc_register_xlat_entry(key, key);
	}
	ADD_STRING(new_persistent_script->script.filename);

#if defined(__AVX__) || defined(__SSE2__)
	/* Align size to 64-byte boundary */
	new_persistent_script->size = (new_persistent_script->size + 63) & ~63;
#endif

	if (new_persistent_script->script.class_table.nNumUsed != new_persistent_script->script.class_table.nNumOfElements) {
		zend_hash_rehash(&new_persistent_script->script.class_table);
	}
	zend_accel_persist_class_table_calc(&new_persistent_script->script.class_table);
	if (new_persistent_script->script.function_table.nNumUsed != new_persistent_script->script.function_table.nNumOfElements) {
		zend_hash_rehash(&new_persistent_script->script.function_table);
	}
	zend_hash_persist_calc(&new_persistent_script->script.function_table);
	ZEND_HASH_FOREACH_BUCKET(&new_persistent_script->script.function_table, p) {
		ZEND_ASSERT(p->key != NULL);
		ADD_INTERNED_STRING(p->key);
		zend_persist_op_array_calc(&p->val);
	} ZEND_HASH_FOREACH_END();
	zend_persist_op_array_calc_ex(&new_persistent_script->script.main_op_array);
	zend_persist_warnings_calc(new_persistent_script);

#if defined(__AVX__) || defined(__SSE2__)
	/* Align size to 64-byte boundary */
	new_persistent_script->arena_size = (new_persistent_script->arena_size + 63) & ~63;
#endif

	new_persistent_script->size += new_persistent_script->arena_size;
	new_persistent_script->corrupted = 0;

	ZCG(current_persistent_script) = NULL;

	return new_persistent_script->size;
}
