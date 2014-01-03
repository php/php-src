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
#include "zend_vm.h"
#include "zend_constants.h"
#include "zend_operators.h"

#define zend_accel_store(p, size) \
	    (p = _zend_shared_memdup((void*)p, size, 1 TSRMLS_CC))
#define zend_accel_memdup(p, size) \
	    _zend_shared_memdup((void*)p, size, 0 TSRMLS_CC)

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
# define zend_accel_memdup_interned_string(str, len) \
	IS_INTERNED(str) ? str : zend_accel_memdup(str, len)

# define zend_accel_store_interned_string(str, len) do { \
		if (!IS_INTERNED(str)) { zend_accel_store(str, len); } \
	} while (0)
#else
# define zend_accel_memdup_interned_string(str, len) \
	zend_accel_memdup(str, len)

# define zend_accel_store_interned_string(str, len) \
	zend_accel_store(str, len)
#endif

typedef void (*zend_persist_func_t)(void * TSRMLS_DC);

static void zend_persist_zval_ptr(zval **zp TSRMLS_DC);

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
static const Bucket *uninitialized_bucket = NULL;
#endif

static void zend_hash_persist(HashTable *ht, void (*pPersistElement)(void *pElement TSRMLS_DC), size_t el_size TSRMLS_DC)
{
	Bucket *p = ht->pListHead;
	uint i;

	while (p) {
		Bucket *q = p;

		/* persist bucket and key */
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		p = zend_accel_memdup(p, sizeof(Bucket));
		if (p->nKeyLength) {
			p->arKey = zend_accel_memdup_interned_string(p->arKey, p->nKeyLength);
		}
#else
		p = zend_accel_memdup(p, sizeof(Bucket) - 1 + p->nKeyLength);
#endif

		/* persist data pointer in bucket */
		if (!p->pDataPtr) {
			zend_accel_store(p->pData, el_size);
		} else {
			/* Update p->pData to point to the new p->pDataPtr address, after the bucket relocation */
			p->pData = &p->pDataPtr;
		}

		/* persist the data itself */
		if (pPersistElement) {
			pPersistElement(p->pData TSRMLS_CC);
		}

		/* update linked lists */
		if (p->pLast) {
			p->pLast->pNext = p;
		}
		if (p->pNext) {
			p->pNext->pLast = p;
		}
		if (p->pListLast) {
			p->pListLast->pListNext = p;
		}
		if (p->pListNext) {
			p->pListNext->pListLast = p;
		}

		p = p->pListNext;

		/* delete the old non-persistent bucket */
		efree(q);
	}

	/* update linked lists */
	if (ht->pListHead) {
		ht->pListHead = zend_shared_alloc_get_xlat_entry(ht->pListHead);
	}
	if (ht->pListTail) {
		ht->pListTail = zend_shared_alloc_get_xlat_entry(ht->pListTail);
	}
	if (ht->pInternalPointer) {
		ht->pInternalPointer = zend_shared_alloc_get_xlat_entry(ht->pInternalPointer);
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	/* Check if HastTable is initialized */
	if (ht->nTableMask) {
#endif
		if (ht->nNumOfElements) {
			/* update hash table */
			for (i = 0; i < ht->nTableSize; i++) {
				if (ht->arBuckets[i]) {
					ht->arBuckets[i] = zend_shared_alloc_get_xlat_entry(ht->arBuckets[i]);
				}
			}
		}
		zend_accel_store(ht->arBuckets, sizeof(Bucket*) * ht->nTableSize);
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	} else {
		ht->arBuckets = (Bucket**)&uninitialized_bucket;
	}
#endif
}

static void zend_persist_zval(zval *z TSRMLS_DC)
{
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	switch (z->type & IS_CONSTANT_TYPE_MASK) {
#else
	switch (z->type & ~IS_CONSTANT_INDEX) {
#endif
		case IS_STRING:
		case IS_CONSTANT:
			zend_accel_store_interned_string(z->value.str.val, z->value.str.len + 1);
			break;
		case IS_ARRAY:
		case IS_CONSTANT_ARRAY:
			zend_accel_store(z->value.ht, sizeof(HashTable));
			zend_hash_persist(z->value.ht, (zend_persist_func_t) zend_persist_zval_ptr, sizeof(zval**) TSRMLS_CC);
			break;
	}
}

static void zend_persist_zval_ptr(zval **zp TSRMLS_DC)
{
	zval *new_ptr = zend_shared_alloc_get_xlat_entry(*zp);

	if (new_ptr) {
		*zp = new_ptr;
	} else {
		/* Attempt to store only if we didn't store this zval_ptr yet */
		zend_accel_store(*zp, sizeof(zval));
		zend_persist_zval(*zp TSRMLS_CC);
	}
}

static void zend_protect_zval(zval *z TSRMLS_DC)
{
	PZ_SET_ISREF_P(z);
	PZ_SET_REFCOUNT_P(z, 2);
}

static void zend_persist_op_array_ex(zend_op_array *op_array, zend_persistent_script* main_persistent_script TSRMLS_DC)
{
	zend_op *persist_ptr;
#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
	int has_jmp = 0;
#endif
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	zend_literal *orig_literals = NULL;
#endif

	if (op_array->type != ZEND_USER_FUNCTION) {
		return;
	}

#if ZEND_EXTENSION_API_NO <= PHP_5_3_X_API_NO
	op_array->size = op_array->last;
#endif

	if (--(*op_array->refcount) == 0) {
		efree(op_array->refcount);
	}
	op_array->refcount = NULL;

	if (op_array->filename) {
		/* do not free! PHP has centralized filename storage, compiler will free it */
		op_array->filename = zend_accel_memdup(op_array->filename, strlen(op_array->filename) + 1);
	}

	if (main_persistent_script) {
		zend_bool orig_in_execution = EG(in_execution);
		zend_op_array *orig_op_array = EG(active_op_array);
		zval offset;

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
		main_persistent_script->early_binding = -1;
#endif
		EG(in_execution) = 1;
		EG(active_op_array) = op_array;
		if (zend_get_constant("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1, &offset TSRMLS_CC)) {
			main_persistent_script->compiler_halt_offset = Z_LVAL(offset);
		}
		EG(active_op_array) = orig_op_array;
		EG(in_execution) = orig_in_execution;
	}

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
	if (op_array->literals) {
		orig_literals = zend_shared_alloc_get_xlat_entry(op_array->literals);
		if (orig_literals) {
			op_array->literals = orig_literals;
		} else {
			zend_literal *p = zend_accel_memdup(op_array->literals, sizeof(zend_literal) * op_array->last_literal);
			zend_literal *end = p + op_array->last_literal;
			orig_literals = op_array->literals;
			op_array->literals = p;
			while (p < end) {
				zend_persist_zval(&p->constant TSRMLS_CC);
				zend_protect_zval(&p->constant TSRMLS_CC);
				p++;
			}
			efree(orig_literals);
		}
	}
#endif

	if ((persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->opcodes))) {
		op_array->opcodes = persist_ptr;
	} else {
		zend_op *new_opcodes = zend_accel_memdup(op_array->opcodes, sizeof(zend_op) * op_array->last);
		zend_op *opline = new_opcodes;
		zend_op *end = new_opcodes + op_array->last;
		int offset = 0;

		for (; opline < end ; opline++, offset++) {
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
				opline->op1.zv = (zval*)((char*)opline->op1.zv + ((char*)op_array->literals - (char*)orig_literals));
#else
				zend_persist_zval(&opline->op1.u.constant TSRMLS_CC);
				zend_protect_zval(&opline->op1.u.constant TSRMLS_CC);
#endif
			}
			if (ZEND_OP2_TYPE(opline) == IS_CONST) {
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
				opline->op2.zv = (zval*)((char*)opline->op2.zv + ((char*)op_array->literals - (char*)orig_literals));
#else
				zend_persist_zval(&opline->op2.u.constant TSRMLS_CC);
				zend_protect_zval(&opline->op2.u.constant TSRMLS_CC);
#endif
			}

#if ZEND_EXTENSION_API_NO < PHP_5_3_X_API_NO
			switch (opline->opcode) {
				case ZEND_JMP:
					has_jmp = 1;
					if (ZEND_DONE_PASS_TWO(op_array)) {
						ZEND_OP1(opline).jmp_addr = &new_opcodes[ZEND_OP1(opline).jmp_addr - op_array->opcodes];
					}
					break;
				case ZEND_JMPZ:
				case ZEND_JMPNZ:
				case ZEND_JMPZ_EX:
				case ZEND_JMPNZ_EX:
					has_jmp = 1;
					if (ZEND_DONE_PASS_TWO(op_array)) {
						ZEND_OP2(opline).jmp_addr = &new_opcodes[ZEND_OP2(opline).jmp_addr - op_array->opcodes];
					}
					break;
				case ZEND_JMPZNZ:
				case ZEND_BRK:
				case ZEND_CONT:
					has_jmp = 1;
					break;
				case ZEND_DECLARE_INHERITED_CLASS:
					if (main_persistent_script && ZCG(accel_directives).inherited_hack) {
					if (!has_jmp &&
					   ((opline + 2) >= end ||
					    (opline + 1)->opcode != ZEND_FETCH_CLASS ||
					    (opline + 2)->opcode != ZEND_ADD_INTERFACE)) {

						zend_uint *opline_num = &main_persistent_script->early_binding;

						while ((int)*opline_num != -1) {
							opline_num = &new_opcodes[*opline_num].result.u.opline_num;
						}
						*opline_num = opline - new_opcodes;
						opline->result.op_type = IS_UNUSED;
						opline->result.u.opline_num = -1;
						opline->opcode = ZEND_DECLARE_INHERITED_CLASS_DELAYED;
						ZEND_VM_SET_OPCODE_HANDLER(opline);
					}
					break;
				}
			}

#else /* if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO */

			if (ZEND_DONE_PASS_TWO(op_array)) {
				/* fix jumps to point to new array */
				switch (opline->opcode) {
					case ZEND_JMP:
					case ZEND_GOTO:
#if ZEND_EXTENSION_API_NO > PHP_5_4_X_API_NO
					case ZEND_FAST_CALL:
#endif
						ZEND_OP1(opline).jmp_addr = &new_opcodes[ZEND_OP1(opline).jmp_addr - op_array->opcodes];
						break;
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_JMP_SET:
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
					case ZEND_JMP_SET_VAR:
#endif
						ZEND_OP2(opline).jmp_addr = &new_opcodes[ZEND_OP2(opline).jmp_addr - op_array->opcodes];
						break;
				}
			}
#endif /* if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO */
		}

		efree(op_array->opcodes);
		op_array->opcodes = new_opcodes;

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (op_array->run_time_cache) {
			efree(op_array->run_time_cache);
			op_array->run_time_cache = NULL;
		}
#endif
	}

	if (op_array->function_name) {
		char *new_name;
		if ((new_name = zend_shared_alloc_get_xlat_entry(op_array->function_name))) {
			op_array->function_name = new_name;
		} else {
			zend_accel_store(op_array->function_name, strlen(op_array->function_name) + 1);
		}
	}

	if (op_array->arg_info) {
		zend_arg_info *new_ptr;
		if ((new_ptr = zend_shared_alloc_get_xlat_entry(op_array->arg_info))) {
			op_array->arg_info = new_ptr;
		} else {
			zend_uint i;

			zend_accel_store(op_array->arg_info, sizeof(zend_arg_info) * op_array->num_args);
			for (i = 0; i < op_array->num_args; i++) {
				if (op_array->arg_info[i].name) {
					zend_accel_store_interned_string(op_array->arg_info[i].name, op_array->arg_info[i].name_len + 1);
				}
				if (op_array->arg_info[i].class_name) {
					zend_accel_store_interned_string(op_array->arg_info[i].class_name, op_array->arg_info[i].class_name_len + 1);
				}
			}
		}
	}

	if (op_array->brk_cont_array) {
		zend_accel_store(op_array->brk_cont_array, sizeof(zend_brk_cont_element) * op_array->last_brk_cont);
	}

	if (op_array->static_variables) {
		zend_hash_persist(op_array->static_variables, (zend_persist_func_t) zend_persist_zval_ptr, sizeof(zval**) TSRMLS_CC);
		zend_accel_store(op_array->static_variables, sizeof(HashTable));
	}

	if (op_array->scope) {
		op_array->scope = zend_shared_alloc_get_xlat_entry(op_array->scope);
	}

	if (op_array->doc_comment) {
		if (ZCG(accel_directives).save_comments) {
			zend_accel_store(op_array->doc_comment, op_array->doc_comment_len + 1);
		} else {
			if (!zend_shared_alloc_get_xlat_entry(op_array->doc_comment)) {
				zend_shared_alloc_register_xlat_entry(op_array->doc_comment, op_array->doc_comment);
				efree((char*)op_array->doc_comment);
			}
			op_array->doc_comment = NULL;
			op_array->doc_comment_len = 0;
		}
	}

	if (op_array->try_catch_array) {
		zend_accel_store(op_array->try_catch_array, sizeof(zend_try_catch_element) * op_array->last_try_catch);
	}

	if (op_array->vars) {
		if ((persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->vars))) {
			op_array->vars = (zend_compiled_variable*)persist_ptr;
		} else {
			int i;
			zend_accel_store(op_array->vars, sizeof(zend_compiled_variable) * op_array->last_var);
			for (i = 0; i < op_array->last_var; i++) {
				zend_accel_store_interned_string(op_array->vars[i].name, op_array->vars[i].name_len + 1);
			}
		}
	}

	/* "prototype" may be undefined if "scope" isn't set */
	if (op_array->scope && op_array->prototype) {
		if ((persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->prototype))) {
			op_array->prototype = (union _zend_function*)persist_ptr;
			/* we use refcount to show that op_array is referenced from several places */
			op_array->prototype->op_array.refcount++;
		}
	} else {
		op_array->prototype = NULL;
	}
}

static void zend_persist_op_array(zend_op_array *op_array TSRMLS_DC)
{
	zend_persist_op_array_ex(op_array, NULL TSRMLS_CC);
}

static void zend_persist_property_info(zend_property_info *prop TSRMLS_DC)
{
	zend_accel_store_interned_string(prop->name, prop->name_length + 1);
	if (prop->doc_comment) {
		if (ZCG(accel_directives).save_comments) {
			zend_accel_store(prop->doc_comment, prop->doc_comment_len + 1);
		} else {
			if (!zend_shared_alloc_get_xlat_entry(prop->doc_comment)) {
				zend_shared_alloc_register_xlat_entry(prop->doc_comment, prop->doc_comment);
				efree((char*)prop->doc_comment);
			}
			prop->doc_comment = NULL;
			prop->doc_comment_len = 0;
		}
	}
}

static void zend_persist_class_entry(zend_class_entry **pce TSRMLS_DC)
{
	zend_class_entry *ce = *pce;

	if (ce->type == ZEND_USER_CLASS) {
		*pce = zend_accel_store(ce, sizeof(zend_class_entry));
		zend_accel_store_interned_string(ce->name, ce->name_length + 1);
		zend_hash_persist(&ce->function_table, (zend_persist_func_t) zend_persist_op_array, sizeof(zend_op_array) TSRMLS_CC);
#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (ce->default_properties_table) {
		    int i;

			zend_accel_store(ce->default_properties_table, sizeof(zval*) * ce->default_properties_count);
			for (i = 0; i < ce->default_properties_count; i++) {
				if (ce->default_properties_table[i]) {
					zend_persist_zval_ptr(&ce->default_properties_table[i] TSRMLS_CC);
				}
			}
		}
		if (ce->default_static_members_table) {
		    int i;

			zend_accel_store(ce->default_static_members_table, sizeof(zval*) * ce->default_static_members_count);
			for (i = 0; i < ce->default_static_members_count; i++) {
				if (ce->default_static_members_table[i]) {
					zend_persist_zval_ptr(&ce->default_static_members_table[i] TSRMLS_CC);
				}
			}
		}
		ce->static_members_table = NULL;
#else
		zend_hash_persist(&ce->default_properties, (zend_persist_func_t) zend_persist_zval_ptr, sizeof(zval**) TSRMLS_CC);
		zend_hash_persist(&ce->default_static_members, (zend_persist_func_t) zend_persist_zval_ptr, sizeof(zval**) TSRMLS_CC);
		ce->static_members = NULL;
#endif
		zend_hash_persist(&ce->constants_table, (zend_persist_func_t) zend_persist_zval_ptr, sizeof(zval**) TSRMLS_CC);

		if (ZEND_CE_FILENAME(ce)) {
			/* do not free! PHP has centralized filename storage, compiler will free it */
			ZEND_CE_FILENAME(ce) = zend_accel_memdup(ZEND_CE_FILENAME(ce), strlen(ZEND_CE_FILENAME(ce)) + 1);
		}
		if (ZEND_CE_DOC_COMMENT(ce)) {
			if (ZCG(accel_directives).save_comments) {
				zend_accel_store(ZEND_CE_DOC_COMMENT(ce), ZEND_CE_DOC_COMMENT_LEN(ce) + 1);
			} else {
				if (!zend_shared_alloc_get_xlat_entry(ZEND_CE_DOC_COMMENT(ce))) {
					zend_shared_alloc_register_xlat_entry(ZEND_CE_DOC_COMMENT(ce), ZEND_CE_DOC_COMMENT(ce));
					efree((char*)ZEND_CE_DOC_COMMENT(ce));
				}
				ZEND_CE_DOC_COMMENT(ce) = NULL;
				ZEND_CE_DOC_COMMENT_LEN(ce) = 0;
			}
		}
		zend_hash_persist(&ce->properties_info, (zend_persist_func_t) zend_persist_property_info, sizeof(zend_property_info) TSRMLS_CC);
		if (ce->num_interfaces && ce->interfaces) {
			efree(ce->interfaces);
		}
		ce->interfaces = NULL; /* will be filled in on fetch */

#if ZEND_EXTENSION_API_NO > PHP_5_3_X_API_NO
		if (ce->num_traits && ce->traits) {
			efree(ce->traits);
		}
		ce->traits = NULL;

		if (ce->trait_aliases) {
			int i = 0;
			while (ce->trait_aliases[i]) {
				if (ce->trait_aliases[i]->trait_method) {
					if (ce->trait_aliases[i]->trait_method->method_name) {
						zend_accel_store(ce->trait_aliases[i]->trait_method->method_name,
							ce->trait_aliases[i]->trait_method->mname_len + 1);
					}
					if (ce->trait_aliases[i]->trait_method->class_name) {
						zend_accel_store(ce->trait_aliases[i]->trait_method->class_name,
							ce->trait_aliases[i]->trait_method->cname_len + 1);
					}
					ce->trait_aliases[i]->trait_method->ce = NULL;
					zend_accel_store(ce->trait_aliases[i]->trait_method,
						sizeof(zend_trait_method_reference));
				}

				if (ce->trait_aliases[i]->alias) {
					zend_accel_store(ce->trait_aliases[i]->alias,
						ce->trait_aliases[i]->alias_len + 1);
				}

#if ZEND_EXTENSION_API_NO <= PHP_5_4_X_API_NO
                ce->trait_aliases[i]->function = NULL;
#endif
				zend_accel_store(ce->trait_aliases[i], sizeof(zend_trait_alias));
				i++;
			}

			zend_accel_store(ce->trait_aliases, sizeof(zend_trait_alias*) * (i + 1));
		}

		if (ce->trait_precedences) {
			int i = 0;

			while (ce->trait_precedences[i]) {
				zend_accel_store(ce->trait_precedences[i]->trait_method->method_name,
					ce->trait_precedences[i]->trait_method->mname_len + 1);
				zend_accel_store(ce->trait_precedences[i]->trait_method->class_name,
					ce->trait_precedences[i]->trait_method->cname_len + 1);
				ce->trait_precedences[i]->trait_method->ce = NULL;
				zend_accel_store(ce->trait_precedences[i]->trait_method,
					sizeof(zend_trait_method_reference));

				if (ce->trait_precedences[i]->exclude_from_classes) {
					int j = 0;

					while (ce->trait_precedences[i]->exclude_from_classes[j]) {
						zend_accel_store(ce->trait_precedences[i]->exclude_from_classes[j],
							strlen((char*)ce->trait_precedences[i]->exclude_from_classes[j]) + 1);
						j++;
					}
					zend_accel_store(ce->trait_precedences[i]->exclude_from_classes,
						sizeof(zend_class_entry*) * (j + 1));
				}

#if ZEND_EXTENSION_API_NO <= PHP_5_4_X_API_NO
                ce->trait_precedences[i]->function = NULL;
#endif
				zend_accel_store(ce->trait_precedences[i], sizeof(zend_trait_precedence));
				i++;
			}
			zend_accel_store(
				ce->trait_precedences, sizeof(zend_trait_precedence*) * (i + 1));
		}
#endif
	}
}

static int zend_update_property_info_ce(zend_property_info *prop TSRMLS_DC)
{
	prop->ce = zend_shared_alloc_get_xlat_entry(prop->ce);
	return 0;
}

static int zend_update_parent_ce(zend_class_entry **pce TSRMLS_DC)
{
	zend_class_entry *ce = *pce;

	if (ce->parent) {
		ce->parent = zend_shared_alloc_get_xlat_entry(ce->parent);
		/* We use refcount to show if the class is used as a parent */
		ce->parent->refcount++;
	}

	/* update methods */
	if (ce->constructor) {
		ce->constructor = zend_shared_alloc_get_xlat_entry(ce->constructor);
		/* we use refcount to show that op_array is referenced from several places */
		ce->constructor->op_array.refcount++;
	}
	if (ce->destructor) {
		ce->destructor = zend_shared_alloc_get_xlat_entry(ce->destructor);
		ce->destructor->op_array.refcount++;
	}
	if (ce->clone) {
		ce->clone = zend_shared_alloc_get_xlat_entry(ce->clone);
		ce->clone->op_array.refcount++;
	}
	if (ce->__get) {
		ce->__get = zend_shared_alloc_get_xlat_entry(ce->__get);
		ce->__get->op_array.refcount++;
	}
	if (ce->__set) {
		ce->__set = zend_shared_alloc_get_xlat_entry(ce->__set);
		ce->__set->op_array.refcount++;
	}
	if (ce->__call) {
		ce->__call = zend_shared_alloc_get_xlat_entry(ce->__call);
		ce->__call->op_array.refcount++;
	}
	if (ce->serialize_func) {
		ce->serialize_func = zend_shared_alloc_get_xlat_entry(ce->serialize_func);
		ce->serialize_func->op_array.refcount++;
	}
	if (ce->unserialize_func) {
		ce->unserialize_func = zend_shared_alloc_get_xlat_entry(ce->unserialize_func);
		ce->unserialize_func->op_array.refcount++;
	}
	if (ce->__isset) {
		ce->__isset = zend_shared_alloc_get_xlat_entry(ce->__isset);
		ce->__isset->op_array.refcount++;
	}
	if (ce->__unset) {
		ce->__unset = zend_shared_alloc_get_xlat_entry(ce->__unset);
		ce->__unset->op_array.refcount++;
	}
	if (ce->__tostring) {
		ce->__tostring = zend_shared_alloc_get_xlat_entry(ce->__tostring);
		ce->__tostring->op_array.refcount++;
	}
#if ZEND_EXTENSION_API_NO >= PHP_5_3_X_API_NO
	if (ce->__callstatic) {
		ce->__callstatic = zend_shared_alloc_get_xlat_entry(ce->__callstatic);
		ce->__callstatic->op_array.refcount++;
	}
#endif
	zend_hash_apply(&ce->properties_info, (apply_func_t) zend_update_property_info_ce TSRMLS_CC);
	return 0;
}

static void zend_accel_persist_class_table(HashTable *class_table TSRMLS_DC)
{
    zend_hash_persist(class_table, (zend_persist_func_t) zend_persist_class_entry, sizeof(zend_class_entry*) TSRMLS_CC);
	zend_hash_apply(class_table, (apply_func_t) zend_update_parent_ce TSRMLS_CC);
}

zend_persistent_script *zend_accel_script_persist(zend_persistent_script *script, char **key, unsigned int key_length TSRMLS_DC)
{
	zend_shared_alloc_clear_xlat_table();
	zend_hash_persist(&script->function_table, (zend_persist_func_t) zend_persist_op_array, sizeof(zend_op_array) TSRMLS_CC);
	zend_accel_persist_class_table(&script->class_table TSRMLS_CC);
	zend_persist_op_array_ex(&script->main_op_array, script TSRMLS_CC);
	*key = zend_accel_memdup(*key, key_length + 1);
	zend_accel_store(script->full_path, script->full_path_len + 1);
	zend_accel_store(script, sizeof(zend_persistent_script));

	return script;
}

int zend_accel_script_persistable(zend_persistent_script *script)
{
	return 1;
}
