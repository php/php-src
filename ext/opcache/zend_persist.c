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
#include "zend_vm.h"
#include "zend_constants.h"
#include "zend_operators.h"
#include "zend_interfaces.h"

#define zend_set_str_gc_flags(str) do { \
	if (file_cache_only) { \
		GC_TYPE_INFO(str) = IS_STRING | (IS_STR_INTERNED << GC_FLAGS_SHIFT); \
	} else { \
		GC_TYPE_INFO(str) = IS_STRING | ((IS_STR_INTERNED | IS_STR_PERMANENT) << GC_FLAGS_SHIFT); \
	} \
} while (0)

#define zend_accel_store_string(str) do { \
		zend_string *new_str = zend_shared_alloc_get_xlat_entry(str); \
		if (new_str) { \
			zend_string_release_ex(str, 0); \
			str = new_str; \
		} else { \
			new_str = zend_shared_memdup_put((void*)str, _ZSTR_STRUCT_SIZE(ZSTR_LEN(str))); \
			zend_string_release_ex(str, 0); \
			str = new_str; \
			zend_string_hash_val(str); \
			zend_set_str_gc_flags(str); \
		} \
    } while (0)
#define zend_accel_memdup_string(str) do { \
		zend_string *new_str = zend_shared_alloc_get_xlat_entry(str); \
		if (new_str) { \
			str = new_str; \
		} else { \
			new_str = zend_shared_memdup_put((void*)str, _ZSTR_STRUCT_SIZE(ZSTR_LEN(str))); \
			str = new_str; \
			zend_string_hash_val(str); \
			zend_set_str_gc_flags(str); \
		} \
    } while (0)
#define zend_accel_store_interned_string(str) do { \
		if (!IS_ACCEL_INTERNED(str)) { \
			zend_accel_store_string(str); \
		} \
	} while (0)
#define zend_accel_memdup_interned_string(str) do { \
		if (!IS_ACCEL_INTERNED(str)) { \
			zend_accel_memdup_string(str); \
		} \
	} while (0)

typedef void (*zend_persist_func_t)(zval*);

static void zend_persist_zval(zval *z);

static const uint32_t uninitialized_bucket[-HT_MIN_MASK] =
	{HT_INVALID_IDX, HT_INVALID_IDX};

static void zend_hash_persist(HashTable *ht)
{
	uint32_t idx, nIndex;
	Bucket *p;

	HT_FLAGS(ht) |= HASH_FLAG_STATIC_KEYS;
	ht->pDestructor = NULL;
	ht->nInternalPointer = 0;

	if (HT_FLAGS(ht) & HASH_FLAG_UNINITIALIZED) {
		if (EXPECTED(!ZCG(current_persistent_script)->corrupted)) {
			HT_SET_DATA_ADDR(ht, &ZCSG(uninitialized_bucket));
		} else {
			HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
		}
		return;
	}
	if (ht->nNumUsed == 0) {
		efree(HT_GET_DATA_ADDR(ht));
		ht->nTableMask = HT_MIN_MASK;
		if (EXPECTED(!ZCG(current_persistent_script)->corrupted)) {
			HT_SET_DATA_ADDR(ht, &ZCSG(uninitialized_bucket));
		} else {
			HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
		}
		HT_FLAGS(ht) |= HASH_FLAG_UNINITIALIZED;
		return;
	}
	if (HT_FLAGS(ht) & HASH_FLAG_PACKED) {
		void *data = HT_GET_DATA_ADDR(ht);
		data = zend_shared_memdup_free(data, HT_USED_SIZE(ht));
		HT_SET_DATA_ADDR(ht, data);
	} else if (ht->nNumUsed > HT_MIN_SIZE && ht->nNumUsed < (uint32_t)(-(int32_t)ht->nTableMask) / 4) {
		/* compact table */
		void *old_data = HT_GET_DATA_ADDR(ht);
		Bucket *old_buckets = ht->arData;
		uint32_t hash_size;

		hash_size = (uint32_t)(-(int32_t)ht->nTableMask);
		while (hash_size >> 2 > ht->nNumUsed) {
			hash_size >>= 1;
		}
		ht->nTableMask = (uint32_t)(-(int32_t)hash_size);
		ZEND_ASSERT(((zend_uintptr_t)ZCG(mem) & 0x7) == 0); /* should be 8 byte aligned */
		HT_SET_DATA_ADDR(ht, ZCG(mem));
		ZCG(mem) = (void*)((char*)ZCG(mem) + ZEND_ALIGNED_SIZE((hash_size * sizeof(uint32_t)) + (ht->nNumUsed * sizeof(Bucket))));
		HT_HASH_RESET(ht);
		memcpy(ht->arData, old_buckets, ht->nNumUsed * sizeof(Bucket));
		efree(old_data);

		/* rehash */
		for (idx = 0; idx < ht->nNumUsed; idx++) {
			p = ht->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			nIndex = p->h | ht->nTableMask;
			Z_NEXT(p->val) = HT_HASH(ht, nIndex);
			HT_HASH(ht, nIndex) = HT_IDX_TO_HASH(idx);
		}
	} else {
		void *data = ZCG(mem);
		void *old_data = HT_GET_DATA_ADDR(ht);

		ZEND_ASSERT(((zend_uintptr_t)ZCG(mem) & 0x7) == 0); /* should be 8 byte aligned */
		ZCG(mem) = (void*)((char*)data + ZEND_ALIGNED_SIZE(HT_USED_SIZE(ht)));
		memcpy(data, old_data, HT_USED_SIZE(ht));
		efree(old_data);
		HT_SET_DATA_ADDR(ht, data);
	}
}

static zend_ast *zend_persist_ast(zend_ast *ast)
{
	uint32_t i;
	zend_ast *node;

	if (ast->kind == ZEND_AST_ZVAL || ast->kind == ZEND_AST_CONSTANT) {
		zend_ast_zval *copy = zend_shared_memdup(ast, sizeof(zend_ast_zval));
		zend_persist_zval(&copy->val);
		node = (zend_ast *) copy;
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		zend_ast_list *copy = zend_shared_memdup(ast,
			sizeof(zend_ast_list) - sizeof(zend_ast *) + sizeof(zend_ast *) * list->children);
		for (i = 0; i < list->children; i++) {
			if (copy->child[i]) {
				copy->child[i] = zend_persist_ast(copy->child[i]);
			}
		}
		node = (zend_ast *) copy;
	} else {
		uint32_t children = zend_ast_get_num_children(ast);
		node = zend_shared_memdup(ast, sizeof(zend_ast) - sizeof(zend_ast *) + sizeof(zend_ast *) * children);
		for (i = 0; i < children; i++) {
			if (node->child[i]) {
				node->child[i] = zend_persist_ast(node->child[i]);
			}
		}
	}

	return node;
}

static void zend_persist_zval(zval *z)
{
	void *new_ptr;

	switch (Z_TYPE_P(z)) {
		case IS_STRING:
			zend_accel_store_interned_string(Z_STR_P(z));
			Z_TYPE_FLAGS_P(z) = 0;
			break;
		case IS_ARRAY:
			new_ptr = zend_shared_alloc_get_xlat_entry(Z_ARR_P(z));
			if (new_ptr) {
				Z_ARR_P(z) = new_ptr;
				Z_TYPE_FLAGS_P(z) = 0;
			} else {
				Bucket *p;

				if (!Z_REFCOUNTED_P(z)) {
					Z_ARR_P(z) = zend_shared_memdup_put(Z_ARR_P(z), sizeof(zend_array));
					zend_hash_persist(Z_ARRVAL_P(z));
					ZEND_HASH_FOREACH_BUCKET(Z_ARRVAL_P(z), p) {
						if (p->key) {
							zend_accel_memdup_interned_string(p->key);
						}
						zend_persist_zval(&p->val);
					} ZEND_HASH_FOREACH_END();
				} else {
					GC_REMOVE_FROM_BUFFER(Z_ARR_P(z));
					Z_ARR_P(z) = zend_shared_memdup_put_free(Z_ARR_P(z), sizeof(zend_array));
					zend_hash_persist(Z_ARRVAL_P(z));
					ZEND_HASH_FOREACH_BUCKET(Z_ARRVAL_P(z), p) {
						if (p->key) {
							zend_accel_store_interned_string(p->key);
						}
						zend_persist_zval(&p->val);
					} ZEND_HASH_FOREACH_END();
					/* make immutable array */
					Z_TYPE_FLAGS_P(z) = 0;
					GC_SET_REFCOUNT(Z_COUNTED_P(z), 2);
					GC_ADD_FLAGS(Z_COUNTED_P(z), IS_ARRAY_IMMUTABLE);
				}
			}
			break;
		case IS_REFERENCE:
			new_ptr = zend_shared_alloc_get_xlat_entry(Z_REF_P(z));
			if (new_ptr) {
				Z_REF_P(z) = new_ptr;
			} else {
				Z_REF_P(z) = zend_shared_memdup_put_free(Z_REF_P(z), sizeof(zend_reference));
				zend_persist_zval(Z_REFVAL_P(z));
			}
			break;
		case IS_CONSTANT_AST:
			new_ptr = zend_shared_alloc_get_xlat_entry(Z_AST_P(z));
			if (new_ptr) {
				Z_AST_P(z) = new_ptr;
				Z_TYPE_FLAGS_P(z) = 0;
			} else {
				zend_ast_ref *old_ref = Z_AST_P(z);
				Z_AST_P(z) = zend_shared_memdup_put(Z_AST_P(z), sizeof(zend_ast_ref));
				zend_persist_ast(GC_AST(old_ref));
				Z_TYPE_FLAGS_P(z) = 0;
				GC_SET_REFCOUNT(Z_COUNTED_P(z), 1);
				efree(old_ref);
			}
			break;
		default:
			ZEND_ASSERT(Z_TYPE_P(z) != IS_OBJECT);
			ZEND_ASSERT(Z_TYPE_P(z) != IS_RESOURCE);
			break;
	}
}

static void zend_persist_op_array_ex(zend_op_array *op_array, zend_persistent_script* main_persistent_script)
{
	zend_op *persist_ptr;
	zval *orig_literals = NULL;

	if (op_array->refcount && --(*op_array->refcount) == 0) {
		efree(op_array->refcount);
	}
	op_array->refcount = NULL;

	if (main_persistent_script) {
		zend_execute_data *orig_execute_data = EG(current_execute_data);
		zend_execute_data fake_execute_data;
		zval *offset;

		memset(&fake_execute_data, 0, sizeof(fake_execute_data));
		fake_execute_data.func = (zend_function*)op_array;
		EG(current_execute_data) = &fake_execute_data;
		if ((offset = zend_get_constant_str("__COMPILER_HALT_OFFSET__", sizeof("__COMPILER_HALT_OFFSET__") - 1)) != NULL) {
			main_persistent_script->compiler_halt_offset = Z_LVAL_P(offset);
		}
		EG(current_execute_data) = orig_execute_data;
	}

	if (op_array->scope) {
		zend_class_entry *scope = zend_shared_alloc_get_xlat_entry(op_array->scope);

		if (scope) {
			op_array->scope = scope;
		}
		if (op_array->prototype) {
			zend_function *ptr = zend_shared_alloc_get_xlat_entry(op_array->prototype);

			if (ptr) {
				op_array->prototype = ptr;
			}
		}

		persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->opcodes);
		if (persist_ptr) {
			op_array->opcodes = persist_ptr;
			if (op_array->static_variables) {
				op_array->static_variables = zend_shared_alloc_get_xlat_entry(op_array->static_variables);
				ZEND_ASSERT(op_array->static_variables != NULL);
			}
			ZEND_MAP_PTR_INIT(op_array->static_variables_ptr, &op_array->static_variables);
			if (op_array->literals) {
				op_array->literals = zend_shared_alloc_get_xlat_entry(op_array->literals);
				ZEND_ASSERT(op_array->literals != NULL);
			}
			if (op_array->function_name && !IS_ACCEL_INTERNED(op_array->function_name)) {
				op_array->function_name = zend_shared_alloc_get_xlat_entry(op_array->function_name);
				ZEND_ASSERT(op_array->function_name != NULL);
			}
			if (op_array->filename) {
				op_array->filename = zend_shared_alloc_get_xlat_entry(op_array->filename);
				ZEND_ASSERT(op_array->filename != NULL);
			}
			if (op_array->arg_info) {
				zend_arg_info *arg_info = op_array->arg_info;
				if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
					arg_info--;
				}
				arg_info = zend_shared_alloc_get_xlat_entry(arg_info);
				ZEND_ASSERT(arg_info != NULL);
				if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
					arg_info++;
				}
				op_array->arg_info = arg_info;
			}
			if (op_array->live_range) {
				op_array->live_range = zend_shared_alloc_get_xlat_entry(op_array->live_range);
				ZEND_ASSERT(op_array->live_range != NULL);
			}
			if (op_array->doc_comment) {
				if (ZCG(accel_directives).save_comments) {
					op_array->doc_comment = zend_shared_alloc_get_xlat_entry(op_array->doc_comment);
					ZEND_ASSERT(op_array->doc_comment != NULL);
				} else {
					op_array->doc_comment = NULL;
				}
			}
			if (op_array->try_catch_array) {
				op_array->try_catch_array = zend_shared_alloc_get_xlat_entry(op_array->try_catch_array);
				ZEND_ASSERT(op_array->try_catch_array != NULL);
			}
			if (op_array->vars) {
				op_array->vars = zend_shared_alloc_get_xlat_entry(op_array->vars);
				ZEND_ASSERT(op_array->vars != NULL);
			}
			ZCG(mem) = (void*)((char*)ZCG(mem) + ZEND_ALIGNED_SIZE(zend_extensions_op_array_persist(op_array, ZCG(mem))));
			return;
		}
	} else {
		/* "prototype" may be undefined if "scope" isn't set */
		op_array->prototype = NULL;
	}

	if (op_array->static_variables) {
		Bucket *p;

		zend_hash_persist(op_array->static_variables);
		ZEND_HASH_FOREACH_BUCKET(op_array->static_variables, p) {
			ZEND_ASSERT(p->key != NULL);
			zend_accel_store_interned_string(p->key);
			zend_persist_zval(&p->val);
		} ZEND_HASH_FOREACH_END();
		op_array->static_variables = zend_shared_memdup_put_free(op_array->static_variables, sizeof(HashTable));
		/* make immutable array */
		GC_SET_REFCOUNT(op_array->static_variables, 2);
		GC_TYPE_INFO(op_array->static_variables) = IS_ARRAY | (IS_ARRAY_IMMUTABLE << GC_FLAGS_SHIFT);
	}
	ZEND_MAP_PTR_INIT(op_array->static_variables_ptr, &op_array->static_variables);

	if (op_array->literals) {
		zval *p, *end;

		orig_literals = op_array->literals;
#if ZEND_USE_ABS_CONST_ADDR
 		p = zend_shared_memdup_put_free(op_array->literals, sizeof(zval) * op_array->last_literal);
#else
 		p = zend_shared_memdup_put(op_array->literals, sizeof(zval) * op_array->last_literal);
#endif
 		end = p + op_array->last_literal;
 		op_array->literals = p;
		while (p < end) {
			zend_persist_zval(p);
			p++;
		}
	}

	{
		zend_op *new_opcodes = zend_shared_memdup_put(op_array->opcodes, sizeof(zend_op) * op_array->last);
		zend_op *opline = new_opcodes;
		zend_op *end = new_opcodes + op_array->last;
		int offset = 0;

		for (; opline < end ; opline++, offset++) {
#if ZEND_USE_ABS_CONST_ADDR
			if (opline->op1_type == IS_CONST) {
				opline->op1.zv = (zval*)((char*)opline->op1.zv + ((char*)op_array->literals - (char*)orig_literals));
				if (opline->opcode == ZEND_SEND_VAL
				 || opline->opcode == ZEND_SEND_VAL_EX
				 || opline->opcode == ZEND_QM_ASSIGN) {
					/* Update handlers to eliminate REFCOUNTED check */
					zend_vm_set_opcode_handler_ex(opline, 1 << Z_TYPE_P(opline->op1.zv), 0, 0);
				}
			}
			if (opline->op2_type == IS_CONST) {
				opline->op2.zv = (zval*)((char*)opline->op2.zv + ((char*)op_array->literals - (char*)orig_literals));
			}
#else
			if (opline->op1_type == IS_CONST) {
				opline->op1.constant =
					(char*)(op_array->literals +
						((zval*)((char*)(op_array->opcodes + (opline - new_opcodes)) +
						(int32_t)opline->op1.constant) - orig_literals)) -
					(char*)opline;
				if (opline->opcode == ZEND_SEND_VAL
				 || opline->opcode == ZEND_SEND_VAL_EX
				 || opline->opcode == ZEND_QM_ASSIGN) {
					zend_vm_set_opcode_handler_ex(opline, 0, 0, 0);
				}
			}
			if (opline->op2_type == IS_CONST) {
				opline->op2.constant =
					(char*)(op_array->literals +
						((zval*)((char*)(op_array->opcodes + (opline - new_opcodes)) +
						(int32_t)opline->op2.constant) - orig_literals)) -
					(char*)opline;
			}
#endif
#if ZEND_USE_ABS_JMP_ADDR
			if (op_array->fn_flags & ZEND_ACC_DONE_PASS_TWO) {
				/* fix jumps to point to new array */
				switch (opline->opcode) {
					case ZEND_JMP:
					case ZEND_FAST_CALL:
						opline->op1.jmp_addr = &new_opcodes[opline->op1.jmp_addr - op_array->opcodes];
						break;
					case ZEND_JMPZNZ:
						/* relative extended_value don't have to be changed */
						/* break omitted intentionally */
					case ZEND_JMPZ:
					case ZEND_JMPNZ:
					case ZEND_JMPZ_EX:
					case ZEND_JMPNZ_EX:
					case ZEND_JMP_SET:
					case ZEND_COALESCE:
					case ZEND_FE_RESET_R:
					case ZEND_FE_RESET_RW:
					case ZEND_ASSERT_CHECK:
						opline->op2.jmp_addr = &new_opcodes[opline->op2.jmp_addr - op_array->opcodes];
						break;
					case ZEND_CATCH:
						if (!(opline->extended_value & ZEND_LAST_CATCH)) {
							opline->op2.jmp_addr = &new_opcodes[opline->op2.jmp_addr - op_array->opcodes];
						}
						break;
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
					case ZEND_SWITCH_LONG:
					case ZEND_SWITCH_STRING:
						/* relative extended_value don't have to be changed */
						break;
				}
			}
#endif
		}

		efree(op_array->opcodes);
		op_array->opcodes = new_opcodes;
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, NULL);
	}

	if (op_array->function_name && !IS_ACCEL_INTERNED(op_array->function_name)) {
		zend_accel_store_interned_string(op_array->function_name);
	}

	if (op_array->filename) {
		/* do not free! PHP has centralized filename storage, compiler will free it */
		zend_accel_memdup_string(op_array->filename);
	}

	if (op_array->arg_info) {
		zend_arg_info *arg_info = op_array->arg_info;
		uint32_t num_args = op_array->num_args;
		uint32_t i;

		if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			arg_info--;
			num_args++;
		}
		if (op_array->fn_flags & ZEND_ACC_VARIADIC) {
			num_args++;
		}
		arg_info = zend_shared_memdup_put_free(arg_info, sizeof(zend_arg_info) * num_args);
		for (i = 0; i < num_args; i++) {
			if (arg_info[i].name) {
				zend_accel_store_interned_string(arg_info[i].name);
			}
			if (ZEND_TYPE_IS_CLASS(arg_info[i].type)) {
				zend_string *type_name = ZEND_TYPE_NAME(arg_info[i].type);
				zend_bool allow_null = ZEND_TYPE_ALLOW_NULL(arg_info[i].type);

				zend_accel_store_interned_string(type_name);
				arg_info[i].type = ZEND_TYPE_ENCODE_CLASS(type_name, allow_null);
			}
		}
		if (op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			arg_info++;
		}
		op_array->arg_info = arg_info;
	}

	if (op_array->live_range) {
		op_array->live_range = zend_shared_memdup_put_free(op_array->live_range, sizeof(zend_live_range) * op_array->last_live_range);
	}

	if (op_array->doc_comment) {
		if (ZCG(accel_directives).save_comments) {
			zend_accel_store_interned_string(op_array->doc_comment);
		} else {
			zend_string_release_ex(op_array->doc_comment, 0);
			op_array->doc_comment = NULL;
		}
	}

	if (op_array->try_catch_array) {
		op_array->try_catch_array = zend_shared_memdup_put_free(op_array->try_catch_array, sizeof(zend_try_catch_element) * op_array->last_try_catch);
	}

	if (op_array->vars) {
		int i;
		op_array->vars = zend_shared_memdup_put_free(op_array->vars, sizeof(zend_string*) * op_array->last_var);
		for (i = 0; i < op_array->last_var; i++) {
			zend_accel_store_interned_string(op_array->vars[i]);
		}
	}

	ZCG(mem) = (void*)((char*)ZCG(mem) + ZEND_ALIGNED_SIZE(zend_extensions_op_array_persist(op_array, ZCG(mem))));
}

static void zend_persist_op_array(zval *zv)
{
	zend_op_array *op_array = Z_PTR_P(zv);

	ZEND_ASSERT(op_array->type == ZEND_USER_FUNCTION);
	op_array = Z_PTR_P(zv) = zend_shared_memdup(Z_PTR_P(zv), sizeof(zend_op_array));
	zend_persist_op_array_ex(op_array, NULL);
	if (!ZCG(current_persistent_script)->corrupted) {
		op_array->fn_flags |= ZEND_ACC_IMMUTABLE;
		ZEND_MAP_PTR_NEW(op_array->run_time_cache);
		if (op_array->static_variables) {
			ZEND_MAP_PTR_NEW(op_array->static_variables_ptr);
		}
	} else {
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, ZCG(arena_mem));
		ZCG(arena_mem) = (void*)(((char*)ZCG(arena_mem)) + ZEND_ALIGNED_SIZE(sizeof(void*)));
		ZEND_MAP_PTR_SET(op_array->run_time_cache, NULL);
	}
}

static void zend_persist_class_method(zval *zv)
{
	zend_op_array *op_array = Z_PTR_P(zv);
	zend_op_array *old_op_array;

	if (op_array->type != ZEND_USER_FUNCTION) {
		ZEND_ASSERT(op_array->type == ZEND_INTERNAL_FUNCTION);
		if (op_array->fn_flags & ZEND_ACC_ARENA_ALLOCATED) {
			old_op_array = zend_shared_alloc_get_xlat_entry(op_array);
			if (old_op_array) {
				Z_PTR_P(zv) = old_op_array;
			} else {
				if (ZCG(is_immutable_class)) {
					op_array = Z_PTR_P(zv) = zend_shared_memdup_put(op_array, sizeof(zend_internal_function));
				} else {
					op_array = Z_PTR_P(zv) = zend_shared_memdup_arena_put(op_array, sizeof(zend_internal_function));
				}
				if (op_array->scope) {
					void *persist_ptr;

					if ((persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->scope))) {
						op_array->scope = (zend_class_entry*)persist_ptr;
					}
					if (op_array->prototype) {
						if ((persist_ptr = zend_shared_alloc_get_xlat_entry(op_array->prototype))) {
							op_array->prototype = (zend_function*)persist_ptr;
						}
					}
				}
			}
		}
		return;
	}

	old_op_array = zend_shared_alloc_get_xlat_entry(op_array);
	if (old_op_array) {
		Z_PTR_P(zv) = old_op_array;
		if (op_array->refcount && --(*op_array->refcount) == 0) {
			efree(op_array->refcount);
		}
		return;
	}
	if (ZCG(is_immutable_class)) {
		op_array = Z_PTR_P(zv) = zend_shared_memdup_put(op_array, sizeof(zend_op_array));
	} else {
		op_array = Z_PTR_P(zv) = zend_shared_memdup_arena_put(op_array, sizeof(zend_op_array));
	}
	zend_persist_op_array_ex(op_array, NULL);
	if (ZCG(is_immutable_class)) {
		op_array->fn_flags |= ZEND_ACC_IMMUTABLE;
		ZEND_MAP_PTR_NEW(op_array->run_time_cache);
		if (op_array->static_variables) {
			ZEND_MAP_PTR_NEW(op_array->static_variables_ptr);
		}
	} else {
		ZEND_MAP_PTR_INIT(op_array->run_time_cache, ZCG(arena_mem));
		ZCG(arena_mem) = (void*)(((char*)ZCG(arena_mem)) + ZEND_ALIGNED_SIZE(sizeof(void*)));
		ZEND_MAP_PTR_SET(op_array->run_time_cache, NULL);
	}
}

static void zend_persist_property_info(zval *zv)
{
	zend_property_info *prop = zend_shared_alloc_get_xlat_entry(Z_PTR_P(zv));
	zend_class_entry *ce;

	if (prop) {
		Z_PTR_P(zv) = prop;
		return;
	}
	if (ZCG(is_immutable_class)) {
		prop = Z_PTR_P(zv) = zend_shared_memdup_put(Z_PTR_P(zv), sizeof(zend_property_info));
	} else {
		prop = Z_PTR_P(zv) = zend_shared_memdup_arena_put(Z_PTR_P(zv), sizeof(zend_property_info));
	}
	ce = zend_shared_alloc_get_xlat_entry(prop->ce);
	if (ce) {
		prop->ce = ce;
	}
	zend_accel_store_interned_string(prop->name);
	if (prop->doc_comment) {
		if (ZCG(accel_directives).save_comments) {
			zend_accel_store_interned_string(prop->doc_comment);
		} else {
			if (!zend_shared_alloc_get_xlat_entry(prop->doc_comment)) {
				zend_shared_alloc_register_xlat_entry(prop->doc_comment, prop->doc_comment);
			}
			zend_string_release_ex(prop->doc_comment, 0);
			prop->doc_comment = NULL;
		}
	}

	if (ZEND_TYPE_IS_NAME(prop->type)) {
		zend_string *class_name = ZEND_TYPE_NAME(prop->type);
		zend_accel_store_interned_string(class_name);
		prop->type = ZEND_TYPE_ENCODE_CLASS(class_name, ZEND_TYPE_ALLOW_NULL(prop->type));
	}
}

static void zend_persist_class_constant(zval *zv)
{
	zend_class_constant *c = zend_shared_alloc_get_xlat_entry(Z_PTR_P(zv));
	zend_class_entry *ce;

	if (c) {
		Z_PTR_P(zv) = c;
		return;
	}
	if (ZCG(is_immutable_class)) {
		c = Z_PTR_P(zv) = zend_shared_memdup_put(Z_PTR_P(zv), sizeof(zend_class_constant));
	} else {
		c = Z_PTR_P(zv) = zend_shared_memdup_arena_put(Z_PTR_P(zv), sizeof(zend_class_constant));
	}
	zend_persist_zval(&c->value);
	ce = zend_shared_alloc_get_xlat_entry(c->ce);
	if (ce) {
		c->ce = ce;
	}
	if (c->doc_comment) {
		if (ZCG(accel_directives).save_comments) {
			zend_string *doc_comment = zend_shared_alloc_get_xlat_entry(c->doc_comment);
			if (doc_comment) {
				c->doc_comment = doc_comment;
			} else {
				zend_accel_store_interned_string(c->doc_comment);
			}
		} else {
			zend_string *doc_comment = zend_shared_alloc_get_xlat_entry(c->doc_comment);
			if (!doc_comment) {
				zend_shared_alloc_register_xlat_entry(c->doc_comment, c->doc_comment);
				zend_string_release_ex(c->doc_comment, 0);
			}
			c->doc_comment = NULL;
		}
	}
}

static void zend_persist_class_entry(zval *zv)
{
	Bucket *p;
	zend_class_entry *ce = Z_PTR_P(zv);

	if (ce->type == ZEND_USER_CLASS) {
		/* The same zend_class_entry may be reused by class_alias */
		zend_class_entry *new_ce = zend_shared_alloc_get_xlat_entry(ce);
		if (new_ce) {
			Z_PTR_P(zv) = new_ce;
			return;
		}
		if ((ce->ce_flags & ZEND_ACC_LINKED)
		 && (ce->ce_flags & ZEND_ACC_CONSTANTS_UPDATED)
		 && (ce->ce_flags & ZEND_ACC_PROPERTY_TYPES_RESOLVED)
		 && !ZCG(current_persistent_script)->corrupted) {
			ZCG(is_immutable_class) = 1;
			ce = Z_PTR_P(zv) = zend_shared_memdup_put(ce, sizeof(zend_class_entry));
			ce->ce_flags |= ZEND_ACC_IMMUTABLE;
		} else {
			ZCG(is_immutable_class) = 0;
			ce = Z_PTR_P(zv) = zend_shared_memdup_arena_put(ce, sizeof(zend_class_entry));
		}
		zend_accel_store_interned_string(ce->name);
		if (ce->parent_name && !(ce->ce_flags & ZEND_ACC_LINKED)) {
			zend_accel_store_interned_string(ce->parent_name);
		}
		zend_hash_persist(&ce->function_table);
		ZEND_HASH_FOREACH_BUCKET(&ce->function_table, p) {
			ZEND_ASSERT(p->key != NULL);
			zend_accel_store_interned_string(p->key);
			zend_persist_class_method(&p->val);
		} ZEND_HASH_FOREACH_END();
		HT_FLAGS(&ce->function_table) &= (HASH_FLAG_UNINITIALIZED | HASH_FLAG_STATIC_KEYS);
		if (ce->default_properties_table) {
		    int i;

			ce->default_properties_table = zend_shared_memdup_free(ce->default_properties_table, sizeof(zval) * ce->default_properties_count);
			for (i = 0; i < ce->default_properties_count; i++) {
				zend_persist_zval(&ce->default_properties_table[i]);
			}
		}
		if (ce->default_static_members_table) {
			int i;
			ce->default_static_members_table = zend_shared_memdup_free(ce->default_static_members_table, sizeof(zval) * ce->default_static_members_count);

			/* Persist only static properties in this class.
			 * Static properties from parent classes will be handled in class_copy_ctor */
			i = (ce->parent && (ce->ce_flags & ZEND_ACC_LINKED)) ? ce->parent->default_static_members_count : 0;
			for (; i < ce->default_static_members_count; i++) {
				zend_persist_zval(&ce->default_static_members_table[i]);
			}
			if (ce->ce_flags & ZEND_ACC_IMMUTABLE) {
				ZEND_MAP_PTR_NEW(ce->static_members_table);
			} else {
				ZEND_MAP_PTR_INIT(ce->static_members_table, &ce->default_static_members_table);
			}
		} else {
			ZEND_MAP_PTR_INIT(ce->static_members_table, &ce->default_static_members_table);
		}

		zend_hash_persist(&ce->constants_table);
		ZEND_HASH_FOREACH_BUCKET(&ce->constants_table, p) {
			ZEND_ASSERT(p->key != NULL);
			zend_accel_store_interned_string(p->key);
			zend_persist_class_constant(&p->val);
		} ZEND_HASH_FOREACH_END();
		HT_FLAGS(&ce->constants_table) &= (HASH_FLAG_UNINITIALIZED | HASH_FLAG_STATIC_KEYS);

		if (ce->info.user.filename) {
			/* do not free! PHP has centralized filename storage, compiler will free it */
			zend_accel_memdup_string(ce->info.user.filename);
		}
		if (ce->info.user.doc_comment) {
			if (ZCG(accel_directives).save_comments) {
				zend_accel_store_interned_string(ce->info.user.doc_comment);
			} else {
				if (!zend_shared_alloc_get_xlat_entry(ce->info.user.doc_comment)) {
					zend_shared_alloc_register_xlat_entry(ce->info.user.doc_comment, ce->info.user.doc_comment);
					zend_string_release_ex(ce->info.user.doc_comment, 0);
				}
				ce->info.user.doc_comment = NULL;
			}
		}
		zend_hash_persist(&ce->properties_info);
		ZEND_HASH_FOREACH_BUCKET(&ce->properties_info, p) {
			ZEND_ASSERT(p->key != NULL);
			zend_accel_store_interned_string(p->key);
			zend_persist_property_info(&p->val);
		} ZEND_HASH_FOREACH_END();
		HT_FLAGS(&ce->properties_info) &= (HASH_FLAG_UNINITIALIZED | HASH_FLAG_STATIC_KEYS);

		if (ce->properties_info_table) {
			int i;

			size_t size = sizeof(zend_property_info *) * ce->default_properties_count;
			ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);
			if (ZCG(is_immutable_class)) {
				ce->properties_info_table = zend_shared_memdup(
					ce->properties_info_table, size);
			} else {
				ce->properties_info_table = zend_shared_memdup_arena(
					ce->properties_info_table, size);
			}

			for (i = 0; i < ce->default_properties_count; i++) {
				if (ce->properties_info_table[i]) {
					ce->properties_info_table[i] = zend_shared_alloc_get_xlat_entry(
						ce->properties_info_table[i]);
				}
			}
		}

		if (ce->num_interfaces && !(ce->ce_flags & ZEND_ACC_LINKED)) {
			uint32_t i = 0;

			for (i = 0; i < ce->num_interfaces; i++) {
				zend_accel_store_interned_string(ce->interface_names[i].name);
				zend_accel_store_interned_string(ce->interface_names[i].lc_name);
			}
			ce->interface_names = zend_shared_memdup_free(ce->interface_names, sizeof(zend_class_name) * ce->num_interfaces);
		}

		if (ce->num_traits) {
			uint32_t i = 0;

			for (i = 0; i < ce->num_traits; i++) {
				zend_accel_store_interned_string(ce->trait_names[i].name);
				zend_accel_store_interned_string(ce->trait_names[i].lc_name);
			}
			ce->trait_names = zend_shared_memdup_free(ce->trait_names, sizeof(zend_class_name) * ce->num_traits);

			i = 0;
			if (ce->trait_aliases) {
				while (ce->trait_aliases[i]) {
					if (ce->trait_aliases[i]->trait_method.method_name) {
						zend_accel_store_interned_string(ce->trait_aliases[i]->trait_method.method_name);
					}
					if (ce->trait_aliases[i]->trait_method.class_name) {
						zend_accel_store_interned_string(ce->trait_aliases[i]->trait_method.class_name);
					}

					if (ce->trait_aliases[i]->alias) {
						zend_accel_store_interned_string(ce->trait_aliases[i]->alias);
					}

					ce->trait_aliases[i] = zend_shared_memdup_free(ce->trait_aliases[i], sizeof(zend_trait_alias));
					i++;
				}

				ce->trait_aliases = zend_shared_memdup_free(ce->trait_aliases, sizeof(zend_trait_alias*) * (i + 1));
			}

			if (ce->trait_precedences) {
				uint32_t j;

				i = 0;
				while (ce->trait_precedences[i]) {
					zend_accel_store_interned_string(ce->trait_precedences[i]->trait_method.method_name);
					zend_accel_store_interned_string(ce->trait_precedences[i]->trait_method.class_name);

					for (j = 0; j < ce->trait_precedences[i]->num_excludes; j++) {
						zend_accel_store_interned_string(ce->trait_precedences[i]->exclude_class_names[j]);
					}

					ce->trait_precedences[i] = zend_shared_memdup_free(ce->trait_precedences[i], sizeof(zend_trait_precedence) + (ce->trait_precedences[i]->num_excludes - 1) * sizeof(zend_string*));
					i++;
				}
				ce->trait_precedences = zend_shared_memdup_free(
					ce->trait_precedences, sizeof(zend_trait_precedence*) * (i + 1));
			}
		}

		if (ce->iterator_funcs_ptr) {
			ce->iterator_funcs_ptr = zend_shared_memdup(ce->iterator_funcs_ptr, sizeof(zend_class_iterator_funcs));
		}
	}
}

static void zend_update_parent_ce(zend_class_entry *ce)
{
	if (ce->ce_flags & ZEND_ACC_LINKED) {
		if (ce->parent) {
			int i, end;
			zend_class_entry *parent = ce->parent;

			if (parent->type == ZEND_USER_CLASS) {
				zend_class_entry *p = zend_shared_alloc_get_xlat_entry(parent);

				if (p) {
					ce->parent = parent = p;
				}
			}

			/* Create indirections to static properties from parent classes */
			i = parent->default_static_members_count - 1;
			while (parent && parent->default_static_members_table) {
				end = parent->parent ? parent->parent->default_static_members_count : 0;
				for (; i >= end; i--) {
					zval *p = &ce->default_static_members_table[i];
					ZVAL_INDIRECT(p, &parent->default_static_members_table[i]);
				}

				parent = parent->parent;
			}
		}

		if (ce->num_interfaces) {
			uint32_t i = 0;

			ce->interfaces = zend_shared_memdup_free(ce->interfaces, sizeof(zend_class_entry*) * ce->num_interfaces);
			for (i = 0; i < ce->num_interfaces; i++) {
				if (ce->interfaces[i]->type == ZEND_USER_CLASS) {
					zend_class_entry *tmp = zend_shared_alloc_get_xlat_entry(ce->interfaces[i]);
					if (tmp != NULL) {
						ce->interfaces[i] = tmp;
					}
				}
			}
		}

		if (ce->iterator_funcs_ptr) {
			memset(ce->iterator_funcs_ptr, 0, sizeof(zend_class_iterator_funcs));
			if (instanceof_function_ex(ce, zend_ce_aggregate, 1)) {
				ce->iterator_funcs_ptr->zf_new_iterator = zend_hash_str_find_ptr(&ce->function_table, "getiterator", sizeof("getiterator") - 1);
			}
			if (instanceof_function_ex(ce, zend_ce_iterator, 1)) {
				ce->iterator_funcs_ptr->zf_rewind = zend_hash_str_find_ptr(&ce->function_table, "rewind", sizeof("rewind") - 1);
				ce->iterator_funcs_ptr->zf_valid = zend_hash_str_find_ptr(&ce->function_table, "valid", sizeof("valid") - 1);
				ce->iterator_funcs_ptr->zf_key = zend_hash_str_find_ptr(&ce->function_table, "key", sizeof("key") - 1);
				ce->iterator_funcs_ptr->zf_current = zend_hash_str_find_ptr(&ce->function_table, "current", sizeof("current") - 1);
				ce->iterator_funcs_ptr->zf_next = zend_hash_str_find_ptr(&ce->function_table, "next", sizeof("next") - 1);
        	}
		}
	}

	if (ce->ce_flags & ZEND_ACC_HAS_TYPE_HINTS) {
		zend_property_info *prop;
		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if (ZEND_TYPE_IS_CE(prop->type)) {
				zend_class_entry *ce = ZEND_TYPE_CE(prop->type);
				if (ce->type == ZEND_USER_CLASS) {
					ce = zend_shared_alloc_get_xlat_entry(ce);
					if (ce) {
						prop->type = ZEND_TYPE_ENCODE_CE(ce, ZEND_TYPE_ALLOW_NULL(prop->type));
					}
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* update methods */
	if (ce->constructor) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->constructor);
		if (tmp != NULL) {
			ce->constructor = tmp;
		}
	}
	if (ce->destructor) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->destructor);
		if (tmp != NULL) {
			ce->destructor = tmp;
		}
	}
	if (ce->clone) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->clone);
		if (tmp != NULL) {
			ce->clone = tmp;
		}
	}
	if (ce->__get) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__get);
		if (tmp != NULL) {
			ce->__get = tmp;
		}
	}
	if (ce->__set) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__set);
		if (tmp != NULL) {
			ce->__set = tmp;
		}
	}
	if (ce->__call) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__call);
		if (tmp != NULL) {
			ce->__call = tmp;
		}
	}
	if (ce->serialize_func) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->serialize_func);
		if (tmp != NULL) {
			ce->serialize_func = tmp;
		}
	}
	if (ce->unserialize_func) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->unserialize_func);
		if (tmp != NULL) {
			ce->unserialize_func = tmp;
		}
	}
	if (ce->__isset) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__isset);
		if (tmp != NULL) {
			ce->__isset = tmp;
		}
	}
	if (ce->__unset) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__unset);
		if (tmp != NULL) {
			ce->__unset = tmp;
		}
	}
	if (ce->__tostring) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__tostring);
		if (tmp != NULL) {
			ce->__tostring = tmp;
		}
	}
	if (ce->__callstatic) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__callstatic);
		if (tmp != NULL) {
			ce->__callstatic = tmp;
		}
	}
	if (ce->__debugInfo) {
		zend_function *tmp = zend_shared_alloc_get_xlat_entry(ce->__debugInfo);
		if (tmp != NULL) {
			ce->__debugInfo = tmp;
		}
	}
}

static void zend_accel_persist_class_table(HashTable *class_table)
{
	Bucket *p;
	zend_class_entry *ce;

    zend_hash_persist(class_table);
	ZEND_HASH_FOREACH_BUCKET(class_table, p) {
		ZEND_ASSERT(p->key != NULL);
		zend_accel_store_interned_string(p->key);
		zend_persist_class_entry(&p->val);
	} ZEND_HASH_FOREACH_END();
    ZEND_HASH_FOREACH_BUCKET(class_table, p) {
		if (EXPECTED(Z_TYPE(p->val) != IS_ALIAS_PTR)) {
			ce = Z_PTR(p->val);
			zend_update_parent_ce(ce);
		}
	} ZEND_HASH_FOREACH_END();
}

zend_persistent_script *zend_accel_script_persist(zend_persistent_script *script, const char **key, unsigned int key_length, int for_shm)
{
	Bucket *p;

	script->mem = ZCG(mem);

	ZEND_ASSERT(((zend_uintptr_t)ZCG(mem) & 0x7) == 0); /* should be 8 byte aligned */

	script = zend_shared_memdup_free(script, sizeof(zend_persistent_script));
	if (key && *key) {
		*key = zend_shared_memdup_put((void*)*key, key_length + 1);
	}

	script->corrupted = 0;
	ZCG(current_persistent_script) = script;

	if (!for_shm) {
		/* script is not going to be saved in SHM */
		script->corrupted = 1;
	}

	zend_accel_store_interned_string(script->script.filename);

#if defined(__AVX__) || defined(__SSE2__)
	/* Align to 64-byte boundary */
	ZCG(mem) = (void*)(((zend_uintptr_t)ZCG(mem) + 63L) & ~63L);
#else
	ZEND_ASSERT(((zend_uintptr_t)ZCG(mem) & 0x7) == 0); /* should be 8 byte aligned */
#endif

	script->arena_mem = ZCG(arena_mem) = ZCG(mem);
	ZCG(mem) = (void*)((char*)ZCG(mem) + script->arena_size);

	zend_map_ptr_extend(ZCSG(map_ptr_last));

	zend_accel_persist_class_table(&script->script.class_table);
	zend_hash_persist(&script->script.function_table);
	ZEND_HASH_FOREACH_BUCKET(&script->script.function_table, p) {
		ZEND_ASSERT(p->key != NULL);
		zend_accel_store_interned_string(p->key);
		zend_persist_op_array(&p->val);
	} ZEND_HASH_FOREACH_END();
	zend_persist_op_array_ex(&script->script.main_op_array, script);

	if (for_shm) {
		ZCSG(map_ptr_last) = CG(map_ptr_last);
	}

	script->corrupted = 0;
	ZCG(current_persistent_script) = NULL;

	return script;
}

int zend_accel_script_persistable(zend_persistent_script *script)
{
	return 1;
}
