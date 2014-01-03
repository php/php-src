/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 The PHP Group                                |
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

#define START_SIZE()       uint memory_used = 0
#define ADD_DUP_SIZE(m,s)  memory_used += zend_shared_memdup_size((void*)m, s)
#define ADD_SIZE(m)        memory_used += ZEND_ALIGNED_SIZE(m)
#define RETURN_SIZE()      return memory_used

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
# define ADD_INTERNED_STRING(str, len) do { \
		const char *tmp = accel_new_interned_string((str), (len), !IS_INTERNED((str)) TSRMLS_CC); \
		if (tmp != (str)) { \
			(str) = (char*)tmp; \
		} else { \
			ADD_DUP_SIZE((str), (len)); \
		} \
	} while (0)
#else
# define ADD_INTERNED_STRING(str, len) ADD_DUP_SIZE((str), (len))
#endif

static uint zend_persist_zval_ptr_calc(zval **zp TSRMLS_DC);
static uint zend_persist_zval_calc(zval *z TSRMLS_DC);

static uint zend_hash_persist_calc(HashTable *ht, int (*pPersistElement)(void *pElement TSRMLS_DC), size_t el_size TSRMLS_DC)
{
	Bucket *p = ht->pListHead;
	START_SIZE();

	while (p) {
		/* persist bucket and key */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		ADD_DUP_SIZE(p, sizeof(Bucket));
		if (p->nKeyLength) {
			const char *tmp = accel_new_interned_string(p->arKey, p->nKeyLength, 0 TSRMLS_CC);
			if (tmp != p->arKey) {
				p->arKey = tmp;
			} else {
				ADD_DUP_SIZE(p->arKey, p->nKeyLength);
			}
		}
#else
		ADD_DUP_SIZE(p, sizeof(Bucket) - 1 + p->nKeyLength);
#endif

		/* persist data pointer in bucket */
		if (!p->pDataPtr) {
			ADD_DUP_SIZE(p->pData, el_size);
		}

		/* persist the data itself */
		if (pPersistElement) {
			ADD_SIZE(pPersistElement(p->pData TSRMLS_CC));
		}

		p = p->pListNext;
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (ht->nTableMask) {
		ADD_DUP_SIZE(ht->arBuckets, sizeof(Bucket*) * ht->nTableSize);
	}
#else
	ADD_DUP_SIZE(ht->arBuckets, sizeof(Bucket*) * ht->nTableSize);
#endif

	RETURN_SIZE();
}

#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
static uint zend_persist_ast_calc(zend_ast *ast TSRMLS_DC)
{
	int i;
	START_SIZE();

	if (ast->kind == ZEND_CONST) {
		ADD_SIZE(sizeof(zend_ast) + sizeof(zval));
		ADD_SIZE(zend_persist_zval_calc(ast->u.val TSRMLS_CC));
	} else {
		ADD_SIZE(sizeof(zend_ast) + sizeof(zend_ast*) * (ast->children - 1));
		for (i = 0; i < ast->children; i++) {
			if ((&ast->u.child)[i]) {
				ADD_SIZE(zend_persist_ast_calc((&ast->u.child)[i] TSRMLS_CC));
			}
		}
	}
	RETURN_SIZE();
}
#endif

static uint zend_persist_zval_calc(zval *z TSRMLS_DC)
{
	START_SIZE();

#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	switch (z->type & IS_CONSTANT_TYPE_MASK) {
#else
	switch (z->type & ~IS_CONSTANT_INDEX) {
#endif
		case IS_STRING:
		case IS_CONSTANT:
			ADD_INTERNED_STRING(Z_STRVAL_P(z), Z_STRLEN_P(z) + 1);
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY:
			ADD_DUP_SIZE(z->value.ht, sizeof(HashTable));
			ADD_SIZE(zend_hash_persist_calc(z->value.ht, (int (*)(void* TSRMLS_DC)) zend_persist_zval_ptr_calc, sizeof(zval**) TSRMLS_CC));
			break;
#if ZEND_EXTENSION_API_NO > PHP_5_5_X_API_NO
		case IS_CONSTANT_AST:
			ADD_SIZE(zend_persist_ast_calc(Z_AST_P(z) TSRMLS_CC));
			break;
#endif
	}
	RETURN_SIZE();
}

static uint zend_persist_zval_ptr_calc(zval **zp TSRMLS_DC)
{
	START_SIZE();
	zval *new_ptr = zend_shared_alloc_get_xlat_entry(*zp);

	if (!new_ptr) {
		ADD_DUP_SIZE(*zp, sizeof(zval));
		ADD_SIZE(zend_persist_zval_calc(*zp TSRMLS_CC));
	}
	RETURN_SIZE();
}

static uint zend_persist_op_array_calc(zend_op_array *op_array TSRMLS_DC)
{
	START_SIZE();

	if (op_array->type != ZEND_USER_FUNCTION) {
		return 0;
	}

	if (op_array->filename) {
		ADD_DUP_SIZE(op_array->filename, strlen(op_array->filename) + 1);
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (op_array->literals && !zend_shared_alloc_get_xlat_entry(op_array->literals)) {
		zend_literal *p = op_array->literals;
		zend_literal *end = p + op_array->last_literal;
		ADD_DUP_SIZE(op_array->literals, sizeof(zend_literal) * op_array->last_literal);
		while (p < end) {
			ADD_SIZE(zend_persist_zval_calc(&p->constant TSRMLS_CC));
			p++;
		}
	}
#endif

	if (!zend_shared_alloc_get_xlat_entry(op_array->opcodes)) {
#if ZEND_EXTENSION_API_NO <= PHP_5_3_X_API_NO
		zend_op *opline = op_array->opcodes;
		zend_op *end = op_array->opcodes + op_array->last;

		ADD_DUP_SIZE(op_array->opcodes, sizeof(zend_op) * op_array->last);
		while (opline<end) {
			if (opline->op1.op_type == IS_CONST) {
				ADD_SIZE(zend_persist_zval_calc(&opline->op1.u.constant TSRMLS_CC));
			}
			if (opline->op2.op_type == IS_CONST) {
				ADD_SIZE(zend_persist_zval_calc(&opline->op2.u.constant TSRMLS_CC));
			}
			opline++;
		}
#else
		ADD_DUP_SIZE(op_array->opcodes, sizeof(zend_op) * op_array->last);
#endif
	}

	if (op_array->function_name) {
		ADD_DUP_SIZE(op_array->function_name, strlen(op_array->function_name) + 1);
    }

	if (op_array->arg_info &&
		!zend_shared_alloc_get_xlat_entry(op_array->arg_info)) {
		zend_uint i;

		ADD_DUP_SIZE(op_array->arg_info, sizeof(zend_arg_info) * op_array->num_args);
		for (i = 0; i < op_array->num_args; i++) {
			if (op_array->arg_info[i].name) {
				ADD_INTERNED_STRING(op_array->arg_info[i].name, op_array->arg_info[i].name_len + 1);
			}
			if (op_array->arg_info[i].class_name) {
				ADD_INTERNED_STRING(op_array->arg_info[i].class_name, op_array->arg_info[i].class_name_len + 1);
			}

		}
	}

	if (op_array->brk_cont_array) {
		ADD_DUP_SIZE(op_array->brk_cont_array, sizeof(zend_brk_cont_element) * op_array->last_brk_cont);
	}

	if (op_array->static_variables) {
		ADD_DUP_SIZE(op_array->static_variables, sizeof(HashTable));
		ADD_SIZE(zend_hash_persist_calc(op_array->static_variables, (int (*)(void* TSRMLS_DC)) zend_persist_zval_ptr_calc, sizeof(zval**) TSRMLS_CC));
	}

	if (ZCG(accel_directives).save_comments && op_array->doc_comment) {
		ADD_DUP_SIZE(op_array->doc_comment, op_array->doc_comment_len + 1);
	}

	if (op_array->try_catch_array) {
		ADD_DUP_SIZE(op_array->try_catch_array, sizeof(zend_try_catch_element) * op_array->last_try_catch);
	}

	if (op_array->vars && !zend_shared_alloc_get_xlat_entry(op_array->vars)) {
		int i;

		ADD_DUP_SIZE(op_array->vars, sizeof(zend_compiled_variable) * op_array->last_var);
		for (i = 0; i < op_array->last_var; i++) {
			ADD_INTERNED_STRING(op_array->vars[i].name, op_array->vars[i].name_len + 1);
		}
	}

	RETURN_SIZE();
}

static uint zend_persist_property_info_calc(zend_property_info *prop TSRMLS_DC)
{
	START_SIZE();
	ADD_INTERNED_STRING(prop->name, prop->name_length + 1);
	if (ZCG(accel_directives).save_comments && prop->doc_comment) {
		ADD_DUP_SIZE(prop->doc_comment, prop->doc_comment_len + 1);
	}
	RETURN_SIZE();
}

static uint zend_persist_class_entry_calc(zend_class_entry **pce TSRMLS_DC)
{
	zend_class_entry *ce = *pce;
	START_SIZE();

	if (ce->type == ZEND_USER_CLASS) {
		ADD_DUP_SIZE(ce, sizeof(zend_class_entry));
		ADD_INTERNED_STRING(ce->name, ce->name_length + 1);
		ADD_SIZE(zend_hash_persist_calc(&ce->function_table, (int (*)(void* TSRMLS_DC)) zend_persist_op_array_calc, sizeof(zend_op_array) TSRMLS_CC));
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (ce->default_properties_table) {
		    int i;

			ADD_SIZE(sizeof(zval*) * ce->default_properties_count);
			for (i = 0; i < ce->default_properties_count; i++) {
				if (ce->default_properties_table[i]) {
					ADD_SIZE(zend_persist_zval_ptr_calc(&ce->default_properties_table[i] TSRMLS_CC));
				}
			}
		}
		if (ce->default_static_members_table) {
		    int i;

			ADD_SIZE(sizeof(zval*) * ce->default_static_members_count);
			for (i = 0; i < ce->default_static_members_count; i++) {
				if (ce->default_static_members_table[i]) {
					ADD_SIZE(zend_persist_zval_ptr_calc(&ce->default_static_members_table[i] TSRMLS_CC));
				}
			}
		}
#else
		ADD_SIZE(zend_hash_persist_calc(&ce->default_properties, (int (*)(void* TSRMLS_DC)) zend_persist_zval_ptr_calc, sizeof(zval**) TSRMLS_CC));
		ADD_SIZE(zend_hash_persist_calc(&ce->default_static_members, (int (*)(void* TSRMLS_DC)) zend_persist_zval_ptr_calc, sizeof(zval**) TSRMLS_CC));
#endif
		ADD_SIZE(zend_hash_persist_calc(&ce->constants_table, (int (*)(void* TSRMLS_DC)) zend_persist_zval_ptr_calc, sizeof(zval**) TSRMLS_CC));

		if (ZEND_CE_FILENAME(ce)) {
			ADD_DUP_SIZE(ZEND_CE_FILENAME(ce), strlen(ZEND_CE_FILENAME(ce)) + 1);
		}
		if (ZCG(accel_directives).save_comments && ZEND_CE_DOC_COMMENT(ce)) {
			ADD_DUP_SIZE(ZEND_CE_DOC_COMMENT(ce), ZEND_CE_DOC_COMMENT_LEN(ce) + 1);
		}

		ADD_SIZE(zend_hash_persist_calc(&ce->properties_info, (int (*)(void* TSRMLS_DC)) zend_persist_property_info_calc, sizeof(zend_property_info) TSRMLS_CC));

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (ce->trait_aliases) {
			int i = 0;
			while (ce->trait_aliases[i]) {
				if (ce->trait_aliases[i]->trait_method) {
					if (ce->trait_aliases[i]->trait_method->method_name) {
						ADD_SIZE(ce->trait_aliases[i]->trait_method->mname_len + 1);
					}
					if (ce->trait_aliases[i]->trait_method->class_name) {
						ADD_SIZE(ce->trait_aliases[i]->trait_method->cname_len + 1);
					}
					ADD_SIZE(sizeof(zend_trait_method_reference));
				}

				if (ce->trait_aliases[i]->alias) {
					ADD_SIZE(ce->trait_aliases[i]->alias_len + 1);
				}
				ADD_SIZE(sizeof(zend_trait_alias));
				i++;
			}
			ADD_SIZE(sizeof(zend_trait_alias*) * (i + 1));
		}

		if (ce->trait_precedences) {
			int i = 0;

			while (ce->trait_precedences[i]) {
				ADD_SIZE(ce->trait_precedences[i]->trait_method->mname_len + 1);
				ADD_SIZE(ce->trait_precedences[i]->trait_method->cname_len + 1);
				ADD_SIZE(sizeof(zend_trait_method_reference));

				if (ce->trait_precedences[i]->exclude_from_classes) {
					int j = 0;

					while (ce->trait_precedences[i]->exclude_from_classes[j]) {
						ADD_SIZE(strlen((char*)ce->trait_precedences[i]->exclude_from_classes[j]) + 1);
						j++;
					}
					ADD_SIZE(sizeof(zend_class_entry*) * (j + 1));
				}
				ADD_SIZE(sizeof(zend_trait_precedence));
				i++;
			}
			ADD_SIZE(sizeof(zend_trait_precedence*) * (i + 1));
		}
#endif
	}
	RETURN_SIZE();
}

static uint zend_accel_persist_class_table_calc(HashTable *class_table TSRMLS_DC)
{
	return zend_hash_persist_calc(class_table, (int (*)(void* TSRMLS_DC)) zend_persist_class_entry_calc, sizeof(zend_class_entry*) TSRMLS_CC);
}

uint zend_accel_script_persist_calc(zend_persistent_script *new_persistent_script, char *key, unsigned int key_length TSRMLS_DC)
{
	START_SIZE();

	ADD_SIZE(zend_hash_persist_calc(&new_persistent_script->function_table, (int (*)(void* TSRMLS_DC)) zend_persist_op_array_calc, sizeof(zend_op_array) TSRMLS_CC));
	ADD_SIZE(zend_accel_persist_class_table_calc(&new_persistent_script->class_table TSRMLS_CC));
	ADD_SIZE(zend_persist_op_array_calc(&new_persistent_script->main_op_array TSRMLS_CC));
	ADD_DUP_SIZE(key, key_length + 1);
	ADD_DUP_SIZE(new_persistent_script->full_path, new_persistent_script->full_path_len + 1);
	ADD_DUP_SIZE(new_persistent_script, sizeof(zend_persistent_script));

	RETURN_SIZE();
}
