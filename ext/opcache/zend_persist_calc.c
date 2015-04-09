/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "ZendAccelerator.h"
#include "zend_persist.h"
#include "zend_extensions.h"
#include "zend_shared_alloc.h"
#include "zend_operators.h"

#define ADD_DUP_SIZE(m,s)  ZCG(current_persistent_script)->size += zend_shared_memdup_size((void*)m, s)
#define ADD_SIZE(m)        ZCG(current_persistent_script)->size += ZEND_ALIGNED_SIZE(m)

#define ADD_ARENA_SIZE(m)        ZCG(current_persistent_script)->arena_size += ZEND_ALIGNED_SIZE(m)

# define ADD_STRING(str) \
		ADD_DUP_SIZE((str), _STR_HEADER_SIZE + (str)->len + 1)
# define ADD_INTERNED_STRING(str, do_free) do { \
		if (!IS_ACCEL_INTERNED(str)) { \
			zend_string *tmp = accel_new_interned_string(str); \
			if (tmp != (str)) { \
				if (do_free) { \
					/*zend_string_release(str);*/ \
				} \
				(str) = tmp; \
			} else { \
				ADD_STRING(str); \
			} \
		} \
	} while (0)

static void zend_persist_zval_calc(zval *z);

static void zend_hash_persist_calc(HashTable *ht, void (*pPersistElement)(zval *pElement))
{
	uint idx;
	Bucket *p;

	if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
		return;
	}

	ADD_SIZE(HT_USED_SIZE(ht));

	for (idx = 0; idx < ht->nNumUsed; idx++) {
		p = ht->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;

		/* persist bucket and key */
		if (p->key) {
			zend_uchar flags = GC_FLAGS(p->key) & ~ (IS_STR_PERSISTENT | IS_STR_INTERNED | IS_STR_PERMANENT);
			ADD_INTERNED_STRING(p->key, 1);
			GC_FLAGS(p->key) |= flags;
		}

		pPersistElement(&p->val);
	}
}

static void zend_persist_ast_calc(zend_ast *ast)
{
	uint32_t i;

	if (ast->kind == ZEND_AST_ZVAL) {
		ADD_SIZE(sizeof(zend_ast_zval));
		zend_persist_zval_calc(zend_ast_get_zval(ast));
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
	zend_uchar flags;
	uint size;

	switch (Z_TYPE_P(z)) {
		case IS_STRING:
		case IS_CONSTANT:
			flags = Z_GC_FLAGS_P(z) & ~ (IS_STR_PERSISTENT | IS_STR_INTERNED | IS_STR_PERMANENT);
			ADD_INTERNED_STRING(Z_STR_P(z), 0);
			if (!Z_REFCOUNTED_P(z)) {
				Z_TYPE_FLAGS_P(z) &= ~ (IS_TYPE_REFCOUNTED | IS_TYPE_COPYABLE);
			}
			Z_GC_FLAGS_P(z) |= flags;
			break;
		case IS_ARRAY:
			size = zend_shared_memdup_size(Z_ARR_P(z), sizeof(zend_array));
			if (size) {
				ADD_SIZE(size);
				zend_hash_persist_calc(Z_ARRVAL_P(z), zend_persist_zval_calc);
			}
			break;
		case IS_REFERENCE:
			size = zend_shared_memdup_size(Z_REF_P(z), sizeof(zend_reference));
			if (size) {
				ADD_SIZE(size);
				zend_persist_zval_calc(Z_REFVAL_P(z));
			}
			break;
		case IS_CONSTANT_AST:
			size = zend_shared_memdup_size(Z_AST_P(z), sizeof(zend_ast_ref));
			if (size) {
				ADD_SIZE(size);
				zend_persist_ast_calc(Z_ASTVAL_P(z));
			}
			break;
	}
}

static void zend_persist_op_array_calc_ex(zend_op_array *op_array)
{
	if (op_array->type != ZEND_USER_FUNCTION) {
		return;
	}

	if (op_array->static_variables) {
		if (!zend_shared_alloc_get_xlat_entry(op_array->static_variables)) {
			HashTable *old = op_array->static_variables;

			ADD_DUP_SIZE(op_array->static_variables, sizeof(HashTable));
			zend_hash_persist_calc(op_array->static_variables, zend_persist_zval_calc);
			zend_shared_alloc_register_xlat_entry(old, op_array->static_variables);
		}
	}

	if (zend_shared_alloc_get_xlat_entry(op_array->opcodes)) {
		/* already stored */
		if (op_array->function_name) {
			zend_string *new_name = zend_shared_alloc_get_xlat_entry(op_array->function_name);
			if (IS_ACCEL_INTERNED(new_name)) {
				op_array->function_name = new_name;
			}
		}
		return;
	}

	if (op_array->literals) {
		zval *p = op_array->literals;
		zval *end = p + op_array->last_literal;
		ADD_DUP_SIZE(op_array->literals, sizeof(zval) * op_array->last_literal);
		while (p < end) {
			zend_persist_zval_calc(p);
			p++;
		}
	}

	ADD_DUP_SIZE(op_array->opcodes, sizeof(zend_op) * op_array->last);

	if (op_array->function_name) {
		zend_string *old_name = op_array->function_name;
		zend_string *new_name = zend_shared_alloc_get_xlat_entry(old_name);

		if (new_name) {
			op_array->function_name = new_name;
		} else {
			ADD_INTERNED_STRING(op_array->function_name, 0);
			zend_shared_alloc_register_xlat_entry(old_name, op_array->function_name);
		}
    }

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
		ADD_DUP_SIZE(op_array->arg_info, sizeof(zend_arg_info) * num_args);
		ADD_DUP_SIZE(arg_info, sizeof(zend_arg_info) * num_args);
		for (i = 0; i < num_args; i++) {
			if (arg_info[i].name) {
				ADD_INTERNED_STRING(arg_info[i].name, 1);
			}
			if (arg_info[i].class_name) {
				ADD_INTERNED_STRING(arg_info[i].class_name, 1);
			}
		}
	}

	if (op_array->brk_cont_array) {
		ADD_DUP_SIZE(op_array->brk_cont_array, sizeof(zend_brk_cont_element) * op_array->last_brk_cont);
	}

	if (ZCG(accel_directives).save_comments && op_array->doc_comment) {
		ADD_STRING(op_array->doc_comment);
	}

	if (op_array->try_catch_array) {
		ADD_DUP_SIZE(op_array->try_catch_array, sizeof(zend_try_catch_element) * op_array->last_try_catch);
	}

	if (op_array->vars) {
		int i;

		ADD_DUP_SIZE(op_array->vars, sizeof(zend_string*) * op_array->last_var);
		for (i = 0; i < op_array->last_var; i++) {
			ADD_INTERNED_STRING(op_array->vars[i], 0);
		}
	}
}

static void zend_persist_op_array_calc(zval *zv)
{
	zend_op_array *op_array = Z_PTR_P(zv);

	if (op_array->type == ZEND_USER_FUNCTION/* &&
	    (!op_array->refcount || *(op_array->refcount) > 1)*/) {
		zend_op_array *old_op_array = zend_shared_alloc_get_xlat_entry(op_array);
		if (old_op_array) {
			Z_PTR_P(zv) = old_op_array;
		} else {
			ADD_ARENA_SIZE(sizeof(zend_op_array));
			zend_persist_op_array_calc_ex(Z_PTR_P(zv));
			zend_shared_alloc_register_xlat_entry(op_array, Z_PTR_P(zv));
		}
	} else {
		ADD_ARENA_SIZE(sizeof(zend_op_array));
		zend_persist_op_array_calc_ex(Z_PTR_P(zv));
	}
}

static void zend_persist_property_info_calc(zval *zv)
{
	zend_property_info *prop = Z_PTR_P(zv);

	if (!zend_shared_alloc_get_xlat_entry(prop)) {
		zend_shared_alloc_register_xlat_entry(prop, prop);
		ADD_ARENA_SIZE(sizeof(zend_property_info));
		ADD_INTERNED_STRING(prop->name, 0);
		if (ZCG(accel_directives).save_comments && prop->doc_comment) {
			ADD_STRING(prop->doc_comment);
		}
	}
}

static void zend_persist_class_entry_calc(zval *zv)
{
	zend_class_entry *ce = Z_PTR_P(zv);

	if (ce->type == ZEND_USER_CLASS) {
		ADD_ARENA_SIZE(sizeof(zend_class_entry));
		ADD_INTERNED_STRING(ce->name, 0);
		zend_hash_persist_calc(&ce->function_table, zend_persist_op_array_calc);
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
				zend_persist_zval_calc(&ce->default_static_members_table[i]);
			}
		}
		zend_hash_persist_calc(&ce->constants_table, zend_persist_zval_calc);

		if (ZEND_CE_FILENAME(ce)) {
			ADD_STRING(ZEND_CE_FILENAME(ce));
		}
		if (ZCG(accel_directives).save_comments && ZEND_CE_DOC_COMMENT(ce)) {
			ADD_STRING(ZEND_CE_DOC_COMMENT(ce));
		}

		zend_hash_persist_calc(&ce->properties_info, zend_persist_property_info_calc);

		if (ce->trait_aliases) {
			int i = 0;
			while (ce->trait_aliases[i]) {
				if (ce->trait_aliases[i]->trait_method) {
					if (ce->trait_aliases[i]->trait_method->method_name) {
						ADD_INTERNED_STRING(ce->trait_aliases[i]->trait_method->method_name, 0);
					}
					if (ce->trait_aliases[i]->trait_method->class_name) {
						ADD_INTERNED_STRING(ce->trait_aliases[i]->trait_method->class_name, 0);
					}
					ADD_SIZE(sizeof(zend_trait_method_reference));
				}

				if (ce->trait_aliases[i]->alias) {
					ADD_INTERNED_STRING(ce->trait_aliases[i]->alias, 0);
				}
				ADD_SIZE(sizeof(zend_trait_alias));
				i++;
			}
			ADD_SIZE(sizeof(zend_trait_alias*) * (i + 1));
		}

		if (ce->trait_precedences) {
			int i = 0;

			while (ce->trait_precedences[i]) {
				ADD_INTERNED_STRING(ce->trait_precedences[i]->trait_method->method_name, 0);
				ADD_INTERNED_STRING(ce->trait_precedences[i]->trait_method->class_name, 0);
				ADD_SIZE(sizeof(zend_trait_method_reference));

				if (ce->trait_precedences[i]->exclude_from_classes) {
					int j = 0;

					while (ce->trait_precedences[i]->exclude_from_classes[j].class_name) {
						ADD_INTERNED_STRING(ce->trait_precedences[i]->exclude_from_classes[j].class_name, 0);
						j++;
					}
					ADD_SIZE(sizeof(zend_class_entry*) * (j + 1));
				}
				ADD_SIZE(sizeof(zend_trait_precedence));
				i++;
			}
			ADD_SIZE(sizeof(zend_trait_precedence*) * (i + 1));
		}
	}
}

static void zend_accel_persist_class_table_calc(HashTable *class_table)
{
	zend_hash_persist_calc(class_table, zend_persist_class_entry_calc);
}

uint zend_accel_script_persist_calc(zend_persistent_script *new_persistent_script, char *key, unsigned int key_length)
{
	new_persistent_script->mem = NULL;
	new_persistent_script->size = 0;
	new_persistent_script->arena_mem = NULL;
	new_persistent_script->arena_size = 0;
	ZCG(current_persistent_script) = new_persistent_script;

	ADD_DUP_SIZE(new_persistent_script, sizeof(zend_persistent_script));
	if (key) {
		ADD_DUP_SIZE(key, key_length + 1);
	}
	ADD_STRING(new_persistent_script->full_path);

#ifdef __SSE2__
	/* Align size to 64-byte boundary */
	new_persistent_script->size = (new_persistent_script->size + 63) & ~63;
#endif

	zend_accel_persist_class_table_calc(&new_persistent_script->class_table);
	zend_hash_persist_calc(&new_persistent_script->function_table, zend_persist_op_array_calc);
	zend_persist_op_array_calc_ex(&new_persistent_script->main_op_array);

#ifdef __SSE2__
	/* Align size to 64-byte boundary */
	new_persistent_script->arena_size = (new_persistent_script->arena_size + 63) & ~63;
#endif

	new_persistent_script->size += new_persistent_script->arena_size;

	ZCG(current_persistent_script) = NULL;

	return new_persistent_script->size;
}
