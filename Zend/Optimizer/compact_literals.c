/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* pass 11
 * - compact literals table
 */

#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"
#include "zend_extensions.h"

#define DEBUG_COMPACT_LITERALS 0

#define LITERAL_CLASS_CONST 1
#define LITERAL_STATIC_METHOD 2
#define LITERAL_STATIC_PROPERTY 3

typedef struct _literal_info {
	uint8_t num_related;
} literal_info;

#define LITERAL_INFO(n, related) do { \
		info[n].num_related = (related); \
	} while (0)

static size_t type_num_classes(const zend_op_array *op_array, uint32_t arg_num)
{
	zend_arg_info *arg_info;
	if (arg_num > 0) {
		if (!(op_array->fn_flags & ZEND_ACC_HAS_TYPE_HINTS)) {
			return 0;
		}
		if (EXPECTED(arg_num <= op_array->num_args)) {
			arg_info = &op_array->arg_info[arg_num-1];
		} else if (UNEXPECTED(op_array->fn_flags & ZEND_ACC_VARIADIC)) {
			arg_info = &op_array->arg_info[op_array->num_args];
		} else {
			return 0;
		}
	} else {
		arg_info = op_array->arg_info - 1;
	}

	if (ZEND_TYPE_IS_COMPLEX(arg_info->type)) {
		if (ZEND_TYPE_HAS_LIST(arg_info->type)) {
			/* Intersection types cannot have nested list types */
			if (ZEND_TYPE_IS_INTERSECTION(arg_info->type)) {
				return ZEND_TYPE_LIST(arg_info->type)->num_types;
			}
			ZEND_ASSERT(ZEND_TYPE_IS_UNION(arg_info->type));
			size_t count = 0;
			zend_type *list_type;

			ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(arg_info->type), list_type) {
				if (ZEND_TYPE_IS_INTERSECTION(*list_type)) {
					count += ZEND_TYPE_LIST(*list_type)->num_types;
				} else {
					ZEND_ASSERT(!ZEND_TYPE_HAS_LIST(*list_type));
					count += 1;
				}
			} ZEND_TYPE_LIST_FOREACH_END();
			return count;
		}
		return 1;
	}

	return 0;
}

static uint32_t add_static_slot(HashTable     *hash,
                                zend_op_array *op_array,
                                uint32_t       op1,
                                uint32_t       op2,
                                uint32_t       kind,
                                uint32_t       *cache_size)
{
	uint32_t ret;
	zval *class_name = &op_array->literals[op1];
	zval *prop_name = &op_array->literals[op2];
	zval *pos, tmp;

	zend_string *key = zend_create_member_string(Z_STR_P(class_name), Z_STR_P(prop_name));
	ZSTR_H(key) = zend_string_hash_func(key);
	ZSTR_H(key) += kind;

	pos = zend_hash_find(hash, key);
	if (pos) {
		ret = Z_LVAL_P(pos);
	} else {
		ret = *cache_size;
		*cache_size += (kind == LITERAL_STATIC_PROPERTY ? 3 : 2) * sizeof(void *);
		ZVAL_LONG(&tmp, ret);
		zend_hash_add(hash, key, &tmp);
	}
	zend_string_release_ex(key, 0);
	return ret;
}

static inline void bias_key(zend_string *key, uint32_t bias)
{
	/* Add a bias to the hash so we can distinguish string keys
	 * that would otherwise be the same. */
	ZSTR_H(key) = zend_string_hash_val(key) + bias;
}

static zend_string *create_str_cache_key(zval *literal, uint8_t num_related)
{
	ZEND_ASSERT(Z_TYPE_P(literal) == IS_STRING);
	if (num_related == 1) {
		return zend_string_copy(Z_STR_P(literal));
	}

	/* Concatenate all the related literals for the cache key. */
	zend_string *key;
	if (num_related == 2) {
		ZEND_ASSERT(Z_TYPE_P(literal + 1) == IS_STRING);
		key = zend_string_concat2(
			Z_STRVAL_P(literal), Z_STRLEN_P(literal),
			Z_STRVAL_P(literal + 1), Z_STRLEN_P(literal + 1));
	} else if (num_related == 3) {
		ZEND_ASSERT(Z_TYPE_P(literal + 1) == IS_STRING && Z_TYPE_P(literal + 2) == IS_STRING);
		key = zend_string_concat3(
			Z_STRVAL_P(literal), Z_STRLEN_P(literal),
			Z_STRVAL_P(literal + 1), Z_STRLEN_P(literal + 1),
			Z_STRVAL_P(literal + 2), Z_STRLEN_P(literal + 2));
	} else {
		ZEND_ASSERT(0 && "Currently not needed");
	}

	bias_key(key, num_related - 1);
	return key;
}

void zend_optimizer_compact_literals(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline, *end;
	int i, j, n, *map;
	uint32_t cache_size;
	zval zv, *pos;
	literal_info *info;
	int l_null = -1;
	int l_false = -1;
	int l_true = -1;
	int l_empty_arr = -1;
	HashTable hash;
	zend_string *key = NULL;
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	int *const_slot, *class_slot, *func_slot, *bind_var_slot, *property_slot, *method_slot;

	if (op_array->last_literal) {
		info = (literal_info*)zend_arena_calloc(&ctx->arena, op_array->last_literal, sizeof(literal_info));

	    /* Mark literals of specific types */
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			switch (opline->opcode) {
				case ZEND_INIT_FCALL_BY_NAME:
					LITERAL_INFO(opline->op2.constant, 2);
					break;
				case ZEND_INIT_NS_FCALL_BY_NAME:
					LITERAL_INFO(opline->op2.constant, 3);
					break;
				case ZEND_INIT_METHOD_CALL:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 1);
					}
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 2);
					}
					break;
				case ZEND_INIT_STATIC_METHOD_CALL:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 2);
					}
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 2);
					}
					break;
				case ZEND_CATCH:
					LITERAL_INFO(opline->op1.constant, 2);
					break;
				case ZEND_FETCH_CONSTANT:
					if (opline->op1.num & IS_CONSTANT_UNQUALIFIED_IN_NAMESPACE) {
						LITERAL_INFO(opline->op2.constant, 3);
					} else {
						LITERAL_INFO(opline->op2.constant, 2);
					}
					break;
				case ZEND_FETCH_CLASS_CONSTANT:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 2);
					}
					LITERAL_INFO(opline->op2.constant, 1);
					break;
				case ZEND_ASSIGN_STATIC_PROP:
				case ZEND_ASSIGN_STATIC_PROP_REF:
				case ZEND_FETCH_STATIC_PROP_R:
				case ZEND_FETCH_STATIC_PROP_W:
				case ZEND_FETCH_STATIC_PROP_RW:
				case ZEND_FETCH_STATIC_PROP_IS:
				case ZEND_FETCH_STATIC_PROP_UNSET:
				case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
				case ZEND_UNSET_STATIC_PROP:
				case ZEND_ISSET_ISEMPTY_STATIC_PROP:
				case ZEND_PRE_INC_STATIC_PROP:
				case ZEND_PRE_DEC_STATIC_PROP:
				case ZEND_POST_INC_STATIC_PROP:
				case ZEND_POST_DEC_STATIC_PROP:
				case ZEND_ASSIGN_STATIC_PROP_OP:
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 2);
					}
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 1);
					}
					break;
				case ZEND_FETCH_CLASS:
				case ZEND_INSTANCEOF:
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 2);
					}
					break;
				case ZEND_NEW:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 2);
					}
					break;
				case ZEND_DECLARE_CLASS:
				case ZEND_DECLARE_CLASS_DELAYED:
					LITERAL_INFO(opline->op1.constant, 2);
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 1);
					}
					break;
				case ZEND_ISSET_ISEMPTY_DIM_OBJ:
				case ZEND_ASSIGN_DIM:
				case ZEND_UNSET_DIM:
				case ZEND_FETCH_DIM_R:
				case ZEND_FETCH_DIM_W:
				case ZEND_FETCH_DIM_RW:
				case ZEND_FETCH_DIM_IS:
				case ZEND_FETCH_DIM_FUNC_ARG:
				case ZEND_FETCH_DIM_UNSET:
				case ZEND_FETCH_LIST_R:
				case ZEND_FETCH_LIST_W:
				case ZEND_ASSIGN_DIM_OP:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 1);
					}
					if (opline->op2_type == IS_CONST) {
						if (Z_EXTRA(op_array->literals[opline->op2.constant]) == ZEND_EXTRA_VALUE) {
							LITERAL_INFO(opline->op2.constant, 2);
						} else {
							LITERAL_INFO(opline->op2.constant, 1);
						}
					}
					break;
				default:
					if (opline->op1_type == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, 1);
					}
					if (opline->op2_type == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, 1);
					}
					break;
			}
			opline++;
		}

#if DEBUG_COMPACT_LITERALS
		{
			fprintf(stderr, "File %s func %s\n", op_array->filename->val,
					op_array->function_name ? op_array->function_name->val : "main");
			fprintf(stderr, "Literals table size %d\n", op_array->last_literal);

			for (int i = 0; i < op_array->last_literal; i++) {
				zend_string *str = zval_get_string(op_array->literals + i);
				fprintf(stderr, "Literal %d, val (%zu):%s\n", i, ZSTR_LEN(str), ZSTR_VAL(str));
				zend_string_release(str);
			}
			fflush(stderr);
		}
#endif

		/* Merge equal constants */
		j = 0;
		zend_hash_init(&hash, op_array->last_literal, NULL, NULL, 0);
		map = (int*)zend_arena_alloc(&ctx->arena, op_array->last_literal * sizeof(int));
		memset(map, 0, op_array->last_literal * sizeof(int));
		for (i = 0; i < op_array->last_literal; i++) {
			if (!info[i].num_related) {
				/* unset literal */
				zval_ptr_dtor_nogc(&op_array->literals[i]);
				continue;
			}
			switch (Z_TYPE(op_array->literals[i])) {
				case IS_NULL:
					ZEND_ASSERT(info[i].num_related == 1);
					if (l_null < 0) {
						l_null = j;
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					map[i] = l_null;
					break;
				case IS_FALSE:
					ZEND_ASSERT(info[i].num_related == 1);
					if (l_false < 0) {
						l_false = j;
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					map[i] = l_false;
					break;
				case IS_TRUE:
					ZEND_ASSERT(info[i].num_related == 1);
					if (l_true < 0) {
						l_true = j;
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					map[i] = l_true;
					break;
				case IS_LONG:
					if (info[i].num_related == 1) {
						if ((pos = zend_hash_index_find(&hash, Z_LVAL(op_array->literals[i]))) != NULL) {
							map[i] = Z_LVAL_P(pos);
						} else {
							map[i] = j;
							ZVAL_LONG(&zv, j);
							zend_hash_index_add_new(&hash, Z_LVAL(op_array->literals[i]), &zv);
							if (i != j) {
								op_array->literals[j] = op_array->literals[i];
								info[j] = info[i];
							}
							j++;
						}
					} else {
						ZEND_ASSERT(info[i].num_related == 2);
						key = zend_string_init(Z_STRVAL(op_array->literals[i+1]), Z_STRLEN(op_array->literals[i+1]), 0);
						bias_key(key, 100 + info[i].num_related - 1);
						if ((pos = zend_hash_find(&hash, key)) != NULL) {
							ZEND_ASSERT(info[Z_LVAL_P(pos)].num_related == 2);
							map[i] = Z_LVAL_P(pos);
							zval_ptr_dtor_nogc(&op_array->literals[i+1]);
						} else {
							map[i] = j;
							ZVAL_LONG(&zv, j);
							zend_hash_add_new(&hash, key, &zv);
							if (i != j) {
								op_array->literals[j] = op_array->literals[i];
								info[j] = info[i];
								op_array->literals[j+1] = op_array->literals[i+1];
								info[j+1] = info[i+1];
							}
							j += 2;
						}
						zend_string_release_ex(key, 0);
						i++;
					}
					break;
				case IS_DOUBLE:
					ZEND_ASSERT(info[i].num_related == 1);
					key = zend_string_init((char*)&Z_DVAL(op_array->literals[i]), sizeof(double), 0);
					bias_key(key, 200);
					if ((pos = zend_hash_find(&hash, key))) {
						map[i] = Z_LVAL_P(pos);
					} else {
						map[i] = j;
						ZVAL_LONG(&zv, j);
						zend_hash_add_new(&hash, key, &zv);
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					zend_string_release_ex(key, 0);
					break;
				case IS_STRING: {
					key = create_str_cache_key(&op_array->literals[i], info[i].num_related);
					if ((pos = zend_hash_find(&hash, key)) != NULL) {
						ZEND_ASSERT(Z_TYPE(op_array->literals[Z_LVAL_P(pos)]) == IS_STRING &&
							info[i].num_related == info[Z_LVAL_P(pos)].num_related);
						zend_string_release_ex(key, 0);
						map[i] = Z_LVAL_P(pos);
						zval_ptr_dtor_nogc(&op_array->literals[i]);
						n = info[i].num_related;
						while (n > 1) {
							i++;
							zval_ptr_dtor_nogc(&op_array->literals[i]);
							n--;
						}
					} else {
						map[i] = j;
						ZVAL_LONG(&zv, j);
						zend_hash_add_new(&hash, key, &zv);
						zend_string_release_ex(key, 0);
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
						n = info[i].num_related;
						while (n > 1) {
							i++;
							if (i != j) op_array->literals[j] = op_array->literals[i];
							j++;
							n--;
						}
					}
					break;
				}
				case IS_ARRAY:
					ZEND_ASSERT(info[i].num_related == 1);
					if (zend_hash_num_elements(Z_ARRVAL(op_array->literals[i])) == 0) {
						if (l_empty_arr < 0) {
							l_empty_arr = j;
							if (i != j) {
								op_array->literals[j] = op_array->literals[i];
								info[j] = info[i];
							}
							j++;
						} else {
							zval_ptr_dtor_nogc(&op_array->literals[i]);
						}
						map[i] = l_empty_arr;
						break;
					}
					ZEND_FALLTHROUGH;
				default:
					/* don't merge other types */
					ZEND_ASSERT(info[i].num_related == 1);
					map[i] = j;
					if (i != j) {
						op_array->literals[j] = op_array->literals[i];
						info[j] = info[i];
					}
					j++;
					break;
			}
		}

		/* Only clean "hash", as it will be reused in the loop below. */
		zend_hash_clean(&hash);
		op_array->last_literal = j;

		const_slot = zend_arena_alloc(&ctx->arena, j * 6 * sizeof(int));
		memset(const_slot, -1, j * 6 * sizeof(int));
		class_slot = const_slot + j;
		func_slot = class_slot + j;
		bind_var_slot = func_slot + j;
		property_slot = bind_var_slot + j;
		method_slot = property_slot + j;

		/* Update opcodes to use new literals table */
		cache_size = zend_op_array_extension_handles * sizeof(void*);
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			if (opline->op1_type == IS_CONST) {
				opline->op1.constant = map[opline->op1.constant];
			}
			if (opline->op2_type == IS_CONST) {
				opline->op2.constant = map[opline->op2.constant];
			}
			switch (opline->opcode) {
				case ZEND_RECV_INIT:
				case ZEND_RECV:
				case ZEND_RECV_VARIADIC:
				{
					size_t num_classes = type_num_classes(op_array, opline->op1.num);
					if (num_classes) {
						opline->extended_value = cache_size;
						cache_size += num_classes * sizeof(void *);
					}
					break;
				}
				case ZEND_VERIFY_RETURN_TYPE:
				{
					size_t num_classes = type_num_classes(op_array, 0);
					if (num_classes) {
						opline->op2.num = cache_size;
						cache_size += num_classes * sizeof(void *);
					}
					break;
				}
				case ZEND_ASSIGN_STATIC_PROP_OP:
					if (opline->op1_type == IS_CONST) {
						// op1 static property
						if (opline->op2_type == IS_CONST) {
							(opline+1)->extended_value = add_static_slot(&hash, op_array,
								opline->op2.constant,
								opline->op1.constant,
								LITERAL_STATIC_PROPERTY,
								&cache_size);
						} else {
							(opline+1)->extended_value = cache_size;
							cache_size += 3 * sizeof(void *);
						}
					} else if (opline->op2_type == IS_CONST) {
						// op2 class
						if (class_slot[opline->op2.constant] >= 0) {
							(opline+1)->extended_value = class_slot[opline->op2.constant];
						} else {
							(opline+1)->extended_value = cache_size;
							class_slot[opline->op2.constant] = cache_size;
							cache_size += sizeof(void *);
						}
					}
					break;
				case ZEND_ASSIGN_OBJ_OP:
					if (opline->op2_type == IS_CONST) {
						// op2 property
						if (opline->op1_type == IS_UNUSED &&
						    property_slot[opline->op2.constant] >= 0) {
							(opline+1)->extended_value = property_slot[opline->op2.constant];
						} else {
							(opline+1)->extended_value = cache_size;
							cache_size += 3 * sizeof(void *);
							if (opline->op1_type == IS_UNUSED) {
								property_slot[opline->op2.constant] = (opline+1)->extended_value;
							}
						}
					}
					break;
				case ZEND_ASSIGN_OBJ:
				case ZEND_ASSIGN_OBJ_REF:
				case ZEND_FETCH_OBJ_R:
				case ZEND_FETCH_OBJ_W:
				case ZEND_FETCH_OBJ_RW:
				case ZEND_FETCH_OBJ_IS:
				case ZEND_FETCH_OBJ_UNSET:
				case ZEND_FETCH_OBJ_FUNC_ARG:
				case ZEND_UNSET_OBJ:
				case ZEND_PRE_INC_OBJ:
				case ZEND_PRE_DEC_OBJ:
				case ZEND_POST_INC_OBJ:
				case ZEND_POST_DEC_OBJ:
					if (opline->op2_type == IS_CONST) {
						// op2 property
						if (opline->op1_type == IS_UNUSED &&
						    property_slot[opline->op2.constant] >= 0) {
							opline->extended_value = property_slot[opline->op2.constant] | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
						} else {
							opline->extended_value = cache_size | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
							cache_size += 3 * sizeof(void *);
							if (opline->op1_type == IS_UNUSED) {
								property_slot[opline->op2.constant] = opline->extended_value & ~ZEND_FETCH_OBJ_FLAGS;
							}
						}
					}
					break;
				case ZEND_ISSET_ISEMPTY_PROP_OBJ:
					if (opline->op2_type == IS_CONST) {
						// op2 property
						if (opline->op1_type == IS_UNUSED &&
						    property_slot[opline->op2.constant] >= 0) {
							opline->extended_value = property_slot[opline->op2.constant] | (opline->extended_value & ZEND_ISEMPTY);
						} else {
							opline->extended_value = cache_size | (opline->extended_value & ZEND_ISEMPTY);
							cache_size += 3 * sizeof(void *);
							if (opline->op1_type == IS_UNUSED) {
								property_slot[opline->op2.constant] = opline->extended_value & ~ZEND_ISEMPTY;
							}
						}
					}
					break;
				case ZEND_INIT_FCALL:
				case ZEND_INIT_FCALL_BY_NAME:
				case ZEND_INIT_NS_FCALL_BY_NAME:
					// op2 func
					if (func_slot[opline->op2.constant] >= 0) {
						opline->result.num = func_slot[opline->op2.constant];
					} else {
						opline->result.num = cache_size;
						cache_size += sizeof(void *);
						func_slot[opline->op2.constant] = opline->result.num;
					}
					break;
				case ZEND_INIT_METHOD_CALL:
					if (opline->op2_type == IS_CONST) {
						// op2 method
						if (opline->op1_type == IS_UNUSED &&
						    method_slot[opline->op2.constant] >= 0) {
							opline->result.num = method_slot[opline->op2.constant];
						} else {
							opline->result.num = cache_size;
							cache_size += 2 * sizeof(void *);
							if (opline->op1_type == IS_UNUSED) {
								method_slot[opline->op2.constant] = opline->result.num;
							}
						}
					}
					break;
				case ZEND_INIT_STATIC_METHOD_CALL:
					if (opline->op2_type == IS_CONST) {
						// op2 static method
						if (opline->op1_type == IS_CONST) {
							opline->result.num = add_static_slot(&hash, op_array,
								opline->op1.constant,
								opline->op2.constant,
								LITERAL_STATIC_METHOD,
								&cache_size);
						} else {
							opline->result.num = cache_size;
							cache_size += 2 * sizeof(void *);
						}
					} else if (opline->op1_type == IS_CONST) {
						// op1 class
						if (class_slot[opline->op1.constant] >= 0) {
							opline->result.num = class_slot[opline->op1.constant];
						} else {
							opline->result.num = cache_size;
							cache_size += sizeof(void *);
							class_slot[opline->op1.constant] = opline->result.num;
						}
					}
					break;
				case ZEND_DEFINED:
					// op1 const
					if (const_slot[opline->op1.constant] >= 0) {
						opline->extended_value = const_slot[opline->op1.constant];
					} else {
						opline->extended_value = cache_size;
						cache_size += sizeof(void *);
						const_slot[opline->op1.constant] = opline->extended_value;
					}
					break;
				case ZEND_FETCH_CONSTANT:
					// op2 const
					if (const_slot[opline->op2.constant] >= 0) {
						opline->extended_value = const_slot[opline->op2.constant];
					} else {
						opline->extended_value = cache_size;
						cache_size += sizeof(void *);
						const_slot[opline->op2.constant] = opline->extended_value;
					}
					break;
				case ZEND_FETCH_CLASS_CONSTANT:
					if (opline->op1_type == IS_CONST) {
						// op1/op2 class_const
						opline->extended_value = add_static_slot(&hash, op_array,
							opline->op1.constant,
							opline->op2.constant,
							LITERAL_CLASS_CONST,
							&cache_size);
					} else {
						opline->extended_value = cache_size;
						cache_size += 2 * sizeof(void *);
					}
					break;
				case ZEND_ASSIGN_STATIC_PROP:
				case ZEND_ASSIGN_STATIC_PROP_REF:
				case ZEND_FETCH_STATIC_PROP_R:
				case ZEND_FETCH_STATIC_PROP_W:
				case ZEND_FETCH_STATIC_PROP_RW:
				case ZEND_FETCH_STATIC_PROP_IS:
				case ZEND_FETCH_STATIC_PROP_UNSET:
				case ZEND_FETCH_STATIC_PROP_FUNC_ARG:
				case ZEND_UNSET_STATIC_PROP:
				case ZEND_ISSET_ISEMPTY_STATIC_PROP:
				case ZEND_PRE_INC_STATIC_PROP:
				case ZEND_PRE_DEC_STATIC_PROP:
				case ZEND_POST_INC_STATIC_PROP:
				case ZEND_POST_DEC_STATIC_PROP:
					if (opline->op1_type == IS_CONST) {
						// op1 static property
						if (opline->op2_type == IS_CONST) {
							opline->extended_value = add_static_slot(&hash, op_array,
								opline->op2.constant,
								opline->op1.constant,
								LITERAL_STATIC_PROPERTY,
								&cache_size) | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
						} else {
							opline->extended_value = cache_size | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
							cache_size += 3 * sizeof(void *);
						}
					} else if (opline->op2_type == IS_CONST) {
						// op2 class
						if (class_slot[opline->op2.constant] >= 0) {
							opline->extended_value = class_slot[opline->op2.constant] | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
						} else {
							opline->extended_value = cache_size | (opline->extended_value & ZEND_FETCH_OBJ_FLAGS);
							class_slot[opline->op2.constant] = cache_size;
							cache_size += sizeof(void *);
						}
					}
					break;
				case ZEND_FETCH_CLASS:
				case ZEND_INSTANCEOF:
					if (opline->op2_type == IS_CONST) {
						// op2 class
						if (class_slot[opline->op2.constant] >= 0) {
							opline->extended_value = class_slot[opline->op2.constant];
						} else {
							opline->extended_value = cache_size;
							cache_size += sizeof(void *);
							class_slot[opline->op2.constant] = opline->extended_value;
						}
					}
					break;
				case ZEND_NEW:
					if (opline->op1_type == IS_CONST) {
						// op1 class
						if (class_slot[opline->op1.constant] >= 0) {
							opline->op2.num = class_slot[opline->op1.constant];
						} else {
							opline->op2.num = cache_size;
							cache_size += sizeof(void *);
							class_slot[opline->op1.constant] = opline->op2.num;
						}
					}
					break;
				case ZEND_CATCH:
					if (opline->op1_type == IS_CONST) {
						// op1 class
						if (class_slot[opline->op1.constant] >= 0) {
							opline->extended_value = class_slot[opline->op1.constant] | (opline->extended_value & ZEND_LAST_CATCH);
						} else {
							opline->extended_value = cache_size | (opline->extended_value & ZEND_LAST_CATCH);
							cache_size += sizeof(void *);
							class_slot[opline->op1.constant] = opline->extended_value & ~ZEND_LAST_CATCH;
						}
					}
					break;
				case ZEND_BIND_GLOBAL:
					// op2 bind var
					if (bind_var_slot[opline->op2.constant] >= 0) {
						opline->extended_value = bind_var_slot[opline->op2.constant];
					} else {
						opline->extended_value = cache_size;
						cache_size += sizeof(void *);
						bind_var_slot[opline->op2.constant] = opline->extended_value;
					}
					break;
				case ZEND_DECLARE_ANON_CLASS:
				case ZEND_DECLARE_CLASS_DELAYED:
					opline->extended_value = cache_size;
					cache_size += sizeof(void *);
					break;
				case ZEND_SEND_VAL:
				case ZEND_SEND_VAL_EX:
				case ZEND_SEND_VAR:
				case ZEND_SEND_VAR_EX:
				case ZEND_SEND_VAR_NO_REF:
				case ZEND_SEND_VAR_NO_REF_EX:
				case ZEND_SEND_REF:
				case ZEND_SEND_FUNC_ARG:
				case ZEND_CHECK_FUNC_ARG:
					if (opline->op2_type == IS_CONST) {
						opline->result.num = cache_size;
						cache_size += 2 * sizeof(void *);
					}
					break;
			}
			opline++;
		}
		op_array->cache_size = cache_size;
		zend_hash_destroy(&hash);
		zend_arena_release(&ctx->arena, checkpoint);

		if (1) {
			opline = op_array->opcodes;
			while (1) {
				if (opline->opcode == ZEND_RECV_INIT) {
					zval *val = &op_array->literals[opline->op2.constant];

					if (Z_TYPE_P(val) == IS_CONSTANT_AST) {
						/* Ensure zval is aligned to 8 bytes */
						op_array->cache_size = ZEND_MM_ALIGNED_SIZE_EX(op_array->cache_size, 8);
						Z_CACHE_SLOT_P(val) = op_array->cache_size;
						op_array->cache_size += sizeof(zval);
					}
				} else if (opline->opcode != ZEND_RECV) {
					break;
				}
				opline++;
			}
		}

#if DEBUG_COMPACT_LITERALS
		{
			fprintf(stderr, "Optimized literals table size %d\n", op_array->last_literal);

			for (int i = 0; i < op_array->last_literal; i++) {
				zend_string *str = zval_get_string(op_array->literals + i);
				fprintf(stderr, "Literal %d, val (%zu):%s\n", i, ZSTR_LEN(str), ZSTR_VAL(str));
				zend_string_release(str);
			}
			fflush(stderr);
		}
#endif
	}
}
