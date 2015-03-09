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
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

/* pass 11
 * - compact literals table
 */

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"

#define DEBUG_COMPACT_LITERALS 0

#define LITERAL_VALUE                        0x0100
#define LITERAL_FUNC                         0x0200
#define LITERAL_CLASS                        0x0300
#define LITERAL_CONST                        0x0400
#define LITERAL_CLASS_CONST                  0x0500
#define LITERAL_STATIC_METHOD                0x0600
#define LITERAL_STATIC_PROPERTY              0x0700
#define LITERAL_METHOD                       0x0800
#define LITERAL_PROPERTY                     0x0900
#define LITERAL_GLOBAL                       0x0A00

#define LITERAL_EX_CLASS                     0x4000
#define LITERAL_EX_OBJ                       0x2000
#define LITERAL_MAY_MERGE                    0x1000
#define LITERAL_KIND_MASK                    0x0f00
#define LITERAL_NUM_RELATED_MASK             0x000f
#define LITERAL_NUM_SLOTS_MASK               0x00f0
#define LITERAL_NUM_SLOTS_SHIFT              4

#define LITERAL_NUM_RELATED(info) (info & LITERAL_NUM_RELATED_MASK)
#define LITERAL_NUM_SLOTS(info)   ((info & LITERAL_NUM_SLOTS_MASK) >> LITERAL_NUM_SLOTS_SHIFT)

typedef struct _literal_info {
	uint32_t  flags; /* bitmask (see defines above) */
	union {
		int    num;   /* variable number or class name literal number */
	} u;
} literal_info;

#define LITERAL_FLAGS(kind, slots, related) \
	((kind) | ((slots) << LITERAL_NUM_SLOTS_SHIFT) | (related))

#define LITERAL_INFO(n, kind, merge, slots, related) do { \
		info[n].flags = (((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
	} while (0)

#define LITERAL_INFO_CLASS(n, kind, merge, slots, related, _num) do { \
		info[n].flags = (LITERAL_EX_CLASS | ((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
		info[n].u.num = (_num); \
	} while (0)

#define LITERAL_INFO_OBJ(n, kind, merge, slots, related, _num) do { \
		info[n].flags = (LITERAL_EX_OBJ | ((merge) ? LITERAL_MAY_MERGE : 0) | LITERAL_FLAGS(kind, slots, related)); \
		info[n].u.num = (_num); \
	} while (0)

static void optimizer_literal_obj_info(literal_info   *info,
                                       zend_uchar      op_type,
                                       znode_op        op,
                                       int             constant,
                                       uint32_t       kind,
                                       uint32_t       slots,
                                       uint32_t       related,
                                       zend_op_array  *op_array)
{
	/* For now we merge only $this object properties and methods.
	 * In general it's also possible to do it for any CV variable as well,
	 * but it would require complex dataflow and/or type analysis.
	 */
	if (Z_TYPE(op_array->literals[constant]) == IS_STRING &&
	    op_type == IS_UNUSED) {
		LITERAL_INFO_OBJ(constant, kind, 1, slots, related, op_array->this_var);
	} else {
		LITERAL_INFO(constant, kind, 0, slots, related);
	}
}

static void optimizer_literal_class_info(literal_info   *info,
                                         zend_uchar      op_type,
                                         znode_op        op,
                                         int             constant,
                                         uint32_t       kind,
                                         uint32_t       slots,
                                         uint32_t       related,
                                         zend_op_array  *op_array)
{
	if (op_type == IS_CONST) {
		LITERAL_INFO_CLASS(constant, kind, 1, slots, related, op.constant);
	} else {
		LITERAL_INFO(constant, kind, 0, slots, related);
	}
}

void zend_optimizer_compact_literals(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_op *opline, *end;
	int i, j, n, *map, cache_size;
	zval zv, *pos;
	literal_info *info;
	int l_null = -1;
	int l_false = -1;
	int l_true = -1;
	HashTable hash;
	zend_string *key = NULL;
	void *checkpoint = zend_arena_checkpoint(ctx->arena);

	if (op_array->last_literal) {
		info = (literal_info*)zend_arena_calloc(&ctx->arena, op_array->last_literal, sizeof(literal_info));

	    /* Mark literals of specific types */
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			switch (opline->opcode) {
				case ZEND_INIT_FCALL:
					LITERAL_INFO(opline->op2.constant, LITERAL_FUNC, 1, 1, 1);
					break;
				case ZEND_INIT_FCALL_BY_NAME:
					LITERAL_INFO(opline->op2.constant, LITERAL_FUNC, 1, 1, 2);
					break;
				case ZEND_INIT_NS_FCALL_BY_NAME:
					LITERAL_INFO(opline->op2.constant, LITERAL_FUNC, 1, 1, 3);
					break;
				case ZEND_INIT_METHOD_CALL:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_obj_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_METHOD, 2, 2,
							op_array);
					}
					break;
				case ZEND_INIT_STATIC_METHOD_CALL:
					if (ZEND_OP1_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
					}
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_class_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_STATIC_METHOD, (ZEND_OP1_TYPE(opline) == IS_CONST) ? 1 : 2, 2,
							op_array);
					}
					break;
				case ZEND_CATCH:
					LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
					break;
				case ZEND_DEFINED:
					LITERAL_INFO(opline->op1.constant, LITERAL_CONST, 1, 1, 2);
					break;
				case ZEND_FETCH_CONSTANT:
					if (ZEND_OP1_TYPE(opline) == IS_UNUSED) {
						if ((opline->extended_value & (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) == (IS_CONSTANT_IN_NAMESPACE|IS_CONSTANT_UNQUALIFIED)) {
							LITERAL_INFO(opline->op2.constant, LITERAL_CONST, 1, 1, 5);
						} else {
							LITERAL_INFO(opline->op2.constant, LITERAL_CONST, 1, 1, 3);
						}
					} else {
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
						}
						optimizer_literal_class_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_CLASS_CONST, (ZEND_OP1_TYPE(opline) == IS_CONST) ? 1 : 2, 1,
							op_array);
					}
					break;
				case ZEND_FETCH_R:
				case ZEND_FETCH_W:
				case ZEND_FETCH_RW:
				case ZEND_FETCH_IS:
				case ZEND_FETCH_UNSET:
				case ZEND_FETCH_FUNC_ARG:
				case ZEND_UNSET_VAR:
				case ZEND_ISSET_ISEMPTY_VAR:
					if (ZEND_OP2_TYPE(opline) == IS_UNUSED) {
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op1.constant, LITERAL_VALUE, 1, 0, 1);
						}
					} else {
						if (ZEND_OP2_TYPE(opline) == IS_CONST) {
							LITERAL_INFO(opline->op2.constant, LITERAL_CLASS, 1, 1, 2);
						}
						if (ZEND_OP1_TYPE(opline) == IS_CONST) {
							optimizer_literal_class_info(
								info,
								opline->op2_type,
								opline->op2,
								opline->op1.constant,
								LITERAL_STATIC_PROPERTY, 2, 1,
								op_array);
						}
					}
					break;
				case ZEND_FETCH_CLASS:
				case ZEND_ADD_INTERFACE:
				case ZEND_ADD_TRAIT:
				case ZEND_INSTANCEOF:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, LITERAL_CLASS, 1, 1, 2);
					}
					break;
				case ZEND_NEW:
					if (ZEND_OP1_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, LITERAL_CLASS, 1, 1, 2);
					}
					break;
				case ZEND_ASSIGN_OBJ:
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
				case ZEND_ISSET_ISEMPTY_PROP_OBJ:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						optimizer_literal_obj_info(
							info,
							opline->op1_type,
							opline->op1,
							opline->op2.constant,
							LITERAL_PROPERTY, 2, 1,
							op_array);
					}
					break;
				case ZEND_ASSIGN_ADD:
				case ZEND_ASSIGN_SUB:
				case ZEND_ASSIGN_MUL:
				case ZEND_ASSIGN_DIV:
				case ZEND_ASSIGN_MOD:
				case ZEND_ASSIGN_SL:
				case ZEND_ASSIGN_SR:
				case ZEND_ASSIGN_CONCAT:
				case ZEND_ASSIGN_BW_OR:
				case ZEND_ASSIGN_BW_AND:
				case ZEND_ASSIGN_BW_XOR:
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						if (opline->extended_value == ZEND_ASSIGN_OBJ) {
							optimizer_literal_obj_info(
								info,
								opline->op1_type,
								opline->op1,
								opline->op2.constant,
								LITERAL_PROPERTY, 2, 1,
								op_array);
						} else {
							LITERAL_INFO(opline->op2.constant, LITERAL_VALUE, 1, 0, 1);
						}
					}
					break;
				case ZEND_BIND_GLOBAL:
					LITERAL_INFO(opline->op2.constant, LITERAL_GLOBAL, 0, 1, 1);
					break;
				default:
					if (ZEND_OP1_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op1.constant, LITERAL_VALUE, 1, 0, 1);
					}
					if (ZEND_OP2_TYPE(opline) == IS_CONST) {
						LITERAL_INFO(opline->op2.constant, LITERAL_VALUE, 1, 0, 1);
					}
					break;
			}
			opline++;
		}

#if DEBUG_COMPACT_LITERALS
		{
			int i, use_copy;
			fprintf(stderr, "File %s func %s\n", op_array->filename,
					op_array->function_name? op_array->function_name : "main");
			fprintf(stderr, "Literlas table size %d\n", op_array->last_literal);

			for (i = 0; i < op_array->last_literal; i++) {
				zval zv = op_array->literals[i].constant;
				use_copy = zend_make_printable_zval(&op_array->literals[i].constant, &zv);
				fprintf(stderr, "Literal %d, val (%d):%s\n", i, Z_STRLEN(zv), Z_STRVAL(zv));
				if (use_copy) {
					zval_dtor(&zv);
				}
			}
			fflush(stderr);
		}
#endif

		/* Merge equal constants */
		j = 0; cache_size = 0;
		zend_hash_init(&hash, op_array->last_literal, NULL, NULL, 0);
		map = (int*)zend_arena_alloc(&ctx->arena, op_array->last_literal * sizeof(int));
		memset(map, 0, op_array->last_literal * sizeof(int));
		for (i = 0; i < op_array->last_literal; i++) {
			if (!info[i].flags) {
				/* unsed literal */
				zval_dtor(&op_array->literals[i]);
				continue;
			}
			switch (Z_TYPE(op_array->literals[i])) {
				case IS_NULL:
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
					break;
				case IS_DOUBLE:
					if ((pos = zend_hash_str_find(&hash, (char*)&Z_DVAL(op_array->literals[i]), sizeof(double))) != NULL) {
						map[i] = Z_LVAL_P(pos);
					} else {
						map[i] = j;
						ZVAL_LONG(&zv, j);
						zend_hash_str_add(&hash, (char*)&Z_DVAL(op_array->literals[i]), sizeof(double), &zv);
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						j++;
					}
					break;
				case IS_STRING:
				case IS_CONSTANT:
					if (info[i].flags & LITERAL_MAY_MERGE) {
						if (info[i].flags & LITERAL_EX_OBJ) {
							int key_len = MAX_LENGTH_OF_LONG + sizeof("->") + Z_STRLEN(op_array->literals[i]);
							key = zend_string_alloc(key_len, 0);
							key->len = snprintf(key->val, key->len-1, "%d->%s", info[i].u.num, Z_STRVAL(op_array->literals[i]));
						} else if (info[i].flags & LITERAL_EX_CLASS) {
							int key_len;
							zval *class_name = &op_array->literals[(info[i].u.num < i) ? map[info[i].u.num] : info[i].u.num];
							key_len = Z_STRLEN_P(class_name) + sizeof("::") + Z_STRLEN(op_array->literals[i]);
							key = zend_string_alloc(key_len, 0);
							memcpy(key->val, Z_STRVAL_P(class_name), Z_STRLEN_P(class_name));
							memcpy(key->val + Z_STRLEN_P(class_name), "::", sizeof("::") - 1);
							memcpy(key->val + Z_STRLEN_P(class_name) + sizeof("::") - 1,
								Z_STRVAL(op_array->literals[i]),
								Z_STRLEN(op_array->literals[i]) + 1);
						} else {
							key = zend_string_init(Z_STRVAL(op_array->literals[i]), Z_STRLEN(op_array->literals[i]), 0);
						}
						key->h = zend_hash_func(key->val, key->len);
						key->h += info[i].flags;
					}
					if ((info[i].flags & LITERAL_MAY_MERGE) &&
						(pos = zend_hash_find(&hash, key)) != NULL &&
					   	Z_TYPE(op_array->literals[i]) == Z_TYPE(op_array->literals[Z_LVAL_P(pos)]) &&
						info[i].flags == info[Z_LVAL_P(pos)].flags) {

						zend_string_release(key);
						map[i] = Z_LVAL_P(pos);
						zval_dtor(&op_array->literals[i]);
						n = LITERAL_NUM_RELATED(info[i].flags);
						while (n > 1) {
							i++;
							zval_dtor(&op_array->literals[i]);
							n--;
						}
					} else {
						map[i] = j;
						if (info[i].flags & LITERAL_MAY_MERGE) {
							ZVAL_LONG(&zv, j);
							zend_hash_add_new(&hash, key, &zv);
							zend_string_release(key);
						}
						if (i != j) {
							op_array->literals[j] = op_array->literals[i];
							info[j] = info[i];
						}
						if (LITERAL_NUM_SLOTS(info[i].flags)) {
							Z_CACHE_SLOT(op_array->literals[j]) = cache_size;
							cache_size += LITERAL_NUM_SLOTS(info[i].flags) * sizeof(void*);
						}
						j++;
						n = LITERAL_NUM_RELATED(info[i].flags);
						while (n > 1) {
							i++;
							if (i != j) op_array->literals[j] = op_array->literals[i];
							j++;
							n--;
						}
					}
					break;
				default:
					/* don't merge other types */
					map[i] = j;
					if (i != j) {
						op_array->literals[j] = op_array->literals[i];
						info[j] = info[i];
					}
					j++;
					break;
			}
		}
		zend_hash_destroy(&hash);
		op_array->last_literal = j;
		op_array->cache_size = cache_size;

	    /* Update opcodes to use new literals table */
		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			if (ZEND_OP1_TYPE(opline) == IS_CONST) {
				opline->op1.constant = map[opline->op1.constant];
			}
			if (ZEND_OP2_TYPE(opline) == IS_CONST) {
				opline->op2.constant = map[opline->op2.constant];
			}
			opline++;
		}
		zend_arena_release(&ctx->arena, checkpoint);

#if DEBUG_COMPACT_LITERALS
		{
			int i, use_copy;
			fprintf(stderr, "Optimized literlas table size %d\n", op_array->last_literal);

			for (i = 0; i < op_array->last_literal; i++) {
				zval zv = op_array->literals[i].constant;
				use_copy = zend_make_printable_zval(&op_array->literals[i].constant, &zv);
				fprintf(stderr, "Literal %d, val (%d):%s\n", i, Z_STRLEN(zv), Z_STRVAL(zv));
				if (use_copy) {
					zval_dtor(&zv);
				}
			}
			fflush(stderr);
		}
#endif
	}
}
