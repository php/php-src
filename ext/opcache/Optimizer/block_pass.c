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

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_execute.h"
#include "zend_vm.h"
#include "zend_bitset.h"
#include "zend_cfg.h"
#include "zend_dump.h"

/* Checks if a constant (like "true") may be replaced by its value */
int zend_optimizer_get_persistent_constant(zend_string *name, zval *result, int copy)
{
	zend_constant *c = zend_hash_find_ptr(EG(zend_constants), name);
	if (c) {
		if ((ZEND_CONSTANT_FLAGS(c) & CONST_PERSISTENT)
		 && !(ZEND_CONSTANT_FLAGS(c) & CONST_DEPRECATED)
		 && (!(ZEND_CONSTANT_FLAGS(c) & CONST_NO_FILE_CACHE)
		  || !(CG(compiler_options) & ZEND_COMPILE_WITH_FILE_CACHE))) {
			ZVAL_COPY_VALUE(result, &c->value);
			if (copy) {
				Z_TRY_ADDREF_P(result);
			}
			return 1;
		} else {
			return 0;
		}
	}

	/* Special constants null/true/false can always be substituted. */
	c = zend_get_special_const(ZSTR_VAL(name), ZSTR_LEN(name));
	if (c) {
		ZVAL_COPY_VALUE(result, &c->value);
		return 1;
	}
	return 0;
}

/* Data dependencies macros */

#define VAR_SOURCE(op) Tsource[VAR_NUM(op.var)]
#define SET_VAR_SOURCE(opline) Tsource[VAR_NUM(opline->result.var)] = opline

static void strip_leading_nops(zend_op_array *op_array, zend_basic_block *b)
{
	zend_op *opcodes = op_array->opcodes;

	do {
		b->start++;
		b->len--;
	} while (b->len > 0 && opcodes[b->start].opcode == ZEND_NOP);
}

static void strip_nops(zend_op_array *op_array, zend_basic_block *b)
{
	uint32_t i, j;

	if (b->len == 0) {
		return;
	}

	if (op_array->opcodes[b->start].opcode == ZEND_NOP) {
		strip_leading_nops(op_array, b);
	}

	if (b->len == 0) {
		return;
	}

	/* strip the inside NOPs */
	i = j = b->start + 1;
	while (i < b->start + b->len) {
		if (op_array->opcodes[i].opcode != ZEND_NOP) {
			if (i != j) {
				op_array->opcodes[j] = op_array->opcodes[i];
			}
			j++;
		}
		i++;
	}
	b->len = j - b->start;
	while (j < i) {
		MAKE_NOP(op_array->opcodes + j);
		j++;
	}
}

static int get_const_switch_target(zend_cfg *cfg, zend_op_array *op_array, zend_basic_block *block, zend_op *opline, zval *val) {
	HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(opline));
	zval *zv;
	if ((opline->opcode == ZEND_SWITCH_LONG && Z_TYPE_P(val) != IS_LONG)
			|| (opline->opcode == ZEND_SWITCH_STRING && Z_TYPE_P(val) != IS_STRING)) {
		/* fallback to next block */
		return block->successors[block->successors_count - 1];
	}
	if (opline->opcode == ZEND_MATCH && Z_TYPE_P(val) != IS_LONG && Z_TYPE_P(val) != IS_STRING) {
		/* always jump to the default arm */
		return block->successors[block->successors_count - 1];
	}
	if (Z_TYPE_P(val) == IS_LONG) {
		zv = zend_hash_index_find(jumptable, Z_LVAL_P(val));
	} else {
		ZEND_ASSERT(Z_TYPE_P(val) == IS_STRING);
		zv = zend_hash_find(jumptable, Z_STR_P(val));
	}
	if (!zv) {
		/* default */
		return block->successors[block->successors_count - (opline->opcode == ZEND_MATCH ? 1 : 2)];
	}
	return cfg->map[ZEND_OFFSET_TO_OPLINE_NUM(op_array, opline, Z_LVAL_P(zv))];
}

static void zend_optimize_block(zend_basic_block *block, zend_op_array *op_array, zend_bitset used_ext, zend_cfg *cfg, zend_op **Tsource, uint32_t *opt_count)
{
	zend_op *opline, *src;
	zend_op *end, *last_op = NULL;

	if (block->len == 0) {
		return;
	}

	if (op_array->opcodes[block->start].opcode == ZEND_NOP) {
		/* remove leading NOPs */
		strip_leading_nops(op_array, block);
	}

	opline = op_array->opcodes + block->start;
	end = opline + block->len;
	while (opline < end) {
		/* Constant Propagation: strip X = QM_ASSIGN(const) */
		if (opline->op1_type == IS_TMP_VAR &&
		    opline->opcode != ZEND_FREE) {
			src = VAR_SOURCE(opline->op1);
			if (src &&
			    src->opcode == ZEND_QM_ASSIGN &&
			    src->op1_type == IS_CONST
			) {
				znode_op op1 = opline->op1;
				if (opline->opcode == ZEND_VERIFY_RETURN_TYPE) {
					COPY_NODE(opline->result, opline->op1);
					COPY_NODE(opline->op1, src->op1);
					VAR_SOURCE(op1) = NULL;
					MAKE_NOP(src);
					++(*opt_count);
				} else {
					zval c;
					ZVAL_COPY(&c, &ZEND_OP1_LITERAL(src));
					if (zend_optimizer_update_op1_const(op_array, opline, &c)) {
						VAR_SOURCE(op1) = NULL;
						literal_dtor(&ZEND_OP1_LITERAL(src));
						MAKE_NOP(src);
						++(*opt_count);
					} else {
						zval_ptr_dtor_nogc(&c);
					}
				}
			}
		}

		/* Constant Propagation: strip X = QM_ASSIGN(const) */
		if (opline->op2_type == IS_TMP_VAR) {
			src = VAR_SOURCE(opline->op2);
			if (src &&
			    src->opcode == ZEND_QM_ASSIGN &&
			    src->op1_type == IS_CONST) {

				znode_op op2 = opline->op2;
				zval c;

				ZVAL_COPY(&c, &ZEND_OP1_LITERAL(src));
				if (zend_optimizer_update_op2_const(op_array, opline, &c)) {
					VAR_SOURCE(op2) = NULL;
					literal_dtor(&ZEND_OP1_LITERAL(src));
					MAKE_NOP(src);
					++(*opt_count);
				} else {
					zval_ptr_dtor_nogc(&c);
				}
			}
		}

		switch (opline->opcode) {
			case ZEND_ECHO:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op1);
					if (src &&
					    src->opcode == ZEND_CAST &&
					    src->extended_value == IS_STRING) {
						/* T = CAST(X, String), ECHO(T) => NOP, ECHO(X) */
						VAR_SOURCE(opline->op1) = NULL;
						COPY_NODE(opline->op1, src->op1);
						MAKE_NOP(src);
						++(*opt_count);
					}
				} else if (opline->op1_type == IS_CONST &&
				           Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_DOUBLE) {
					if (last_op == opline - 1) {
						/* compress consecutive ECHO's.
						 * Float to string conversion may be affected by current
						 * locale setting.
						 */
						int l, old_len;

						if (Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_STRING) {
							convert_to_string(&ZEND_OP1_LITERAL(opline));
						}
						if (Z_TYPE(ZEND_OP1_LITERAL(last_op)) != IS_STRING) {
							convert_to_string(&ZEND_OP1_LITERAL(last_op));
						}
						old_len = Z_STRLEN(ZEND_OP1_LITERAL(last_op));
						l = old_len + Z_STRLEN(ZEND_OP1_LITERAL(opline));
						if (!Z_REFCOUNTED(ZEND_OP1_LITERAL(last_op))) {
							zend_string *tmp = zend_string_alloc(l, 0);
							memcpy(ZSTR_VAL(tmp), Z_STRVAL(ZEND_OP1_LITERAL(last_op)), old_len);
							Z_STR(ZEND_OP1_LITERAL(last_op)) = tmp;
						} else {
							Z_STR(ZEND_OP1_LITERAL(last_op)) = zend_string_extend(Z_STR(ZEND_OP1_LITERAL(last_op)), l, 0);
						}
						Z_TYPE_INFO(ZEND_OP1_LITERAL(last_op)) = IS_STRING_EX;
						memcpy(Z_STRVAL(ZEND_OP1_LITERAL(last_op)) + old_len, Z_STRVAL(ZEND_OP1_LITERAL(opline)), Z_STRLEN(ZEND_OP1_LITERAL(opline)));
						Z_STRVAL(ZEND_OP1_LITERAL(last_op))[l] = '\0';
						zval_ptr_dtor_nogc(&ZEND_OP1_LITERAL(opline));
						ZVAL_STR(&ZEND_OP1_LITERAL(opline), zend_new_interned_string(Z_STR(ZEND_OP1_LITERAL(last_op))));
						ZVAL_NULL(&ZEND_OP1_LITERAL(last_op));
						MAKE_NOP(last_op);
						++(*opt_count);
					}
					last_op = opline;
				}
				break;

			case ZEND_FREE:
				if (opline->op1_type == IS_TMP_VAR) {
					src = VAR_SOURCE(opline->op1);
					if (src) {
						switch (src->opcode) {
							case ZEND_BOOL:
							case ZEND_BOOL_NOT:
								/* T = BOOL(X), FREE(T) => T = BOOL(X) */
								/* The remaining BOOL is removed by a separate optimization */
								VAR_SOURCE(opline->op1) = NULL;
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_ASSIGN:
							case ZEND_ASSIGN_DIM:
							case ZEND_ASSIGN_OBJ:
							case ZEND_ASSIGN_STATIC_PROP:
							case ZEND_ASSIGN_OP:
							case ZEND_ASSIGN_DIM_OP:
							case ZEND_ASSIGN_OBJ_OP:
							case ZEND_ASSIGN_STATIC_PROP_OP:
							case ZEND_PRE_INC:
							case ZEND_PRE_DEC:
							case ZEND_PRE_INC_OBJ:
							case ZEND_PRE_DEC_OBJ:
							case ZEND_PRE_INC_STATIC_PROP:
							case ZEND_PRE_DEC_STATIC_PROP:
								src->result_type = IS_UNUSED;
								VAR_SOURCE(opline->op1) = NULL;
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							default:
								break;
						}
					}
				} else if (opline->op1_type == IS_VAR) {
					src = VAR_SOURCE(opline->op1);
					/* V = OP, FREE(V) => OP. NOP */
					if (src &&
					    src->opcode != ZEND_FETCH_R &&
					    src->opcode != ZEND_FETCH_STATIC_PROP_R &&
					    src->opcode != ZEND_FETCH_DIM_R &&
					    src->opcode != ZEND_FETCH_OBJ_R &&
					    src->opcode != ZEND_NEW &&
					    src->opcode != ZEND_FETCH_THIS) {
						src->result_type = IS_UNUSED;
						MAKE_NOP(opline);
						++(*opt_count);
						if (src->opcode == ZEND_QM_ASSIGN) {
							if (src->op1_type & (IS_VAR|IS_TMP_VAR)) {
								src->opcode = ZEND_FREE;
							} else {
								MAKE_NOP(src);
							}
						}
					}
				}
				break;

#if 0
		/* pre-evaluate functions:
		   constant(x)
		   function_exists(x)
		   extension_loaded(x)
		   BAD: interacts badly with Accelerator
		*/
		if((opline->op1_type & IS_VAR) &&
		   VAR_SOURCE(opline->op1) && VAR_SOURCE(opline->op1)->opcode == ZEND_DO_CF_FCALL &&
		   VAR_SOURCE(opline->op1)->extended_value == 1) {
			zend_op *fcall = VAR_SOURCE(opline->op1);
			zend_op *sv = fcall-1;
			if(sv >= block->start_opline && sv->opcode == ZEND_SEND_VAL &&
			   sv->op1_type == IS_CONST && Z_TYPE(OPLINE_OP1_LITERAL(sv)) == IS_STRING &&
			   Z_LVAL(OPLINE_OP2_LITERAL(sv)) == 1
			   ) {
				zval *arg = &OPLINE_OP1_LITERAL(sv);
				char *fname = FUNCTION_CACHE->funcs[Z_LVAL(ZEND_OP1_LITERAL(fcall))].function_name;
				int flen = FUNCTION_CACHE->funcs[Z_LVAL(ZEND_OP1_LITERAL(fcall))].name_len;
				if((flen == sizeof("function_exists")-1 && zend_binary_strcasecmp(fname, flen, "function_exists", sizeof("function_exists")-1) == 0) ||
						  (flen == sizeof("is_callable")-1 && zend_binary_strcasecmp(fname, flen, "is_callable", sizeof("is_callable")-1) == 0)
						  ) {
					zend_function *function;
					if((function = zend_hash_find_ptr(EG(function_table), Z_STR_P(arg))) != NULL) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						LITERAL_BOOL(opline->op1, 1);
						opline->op1_type = IS_CONST;
					}
				} else if(flen == sizeof("constant")-1 && zend_binary_strcasecmp(fname, flen, "constant", sizeof("constant")-1) == 0) {
					zval c;
					if(zend_optimizer_get_persistent_constant(Z_STR_P(arg), &c, 1 ELS_CC) != 0) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						ZEND_OP1_LITERAL(opline) = zend_optimizer_add_literal(op_array, &c);
						/* no copy ctor - get already copied it */
						opline->op1_type = IS_CONST;
					}
				} else if(flen == sizeof("extension_loaded")-1 && zend_binary_strcasecmp(fname, flen, "extension_loaded", sizeof("extension_loaded")-1) == 0) {
					if(zend_hash_exists(&module_registry, Z_STR_P(arg))) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						LITERAL_BOOL(opline->op1, 1);
						opline->op1_type = IS_CONST;
					}
				}
			}
		}
#endif

			case ZEND_FETCH_LIST_R:
			case ZEND_FETCH_LIST_W:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					/* LIST variable will be deleted later by FREE */
					Tsource[VAR_NUM(opline->op1.var)] = NULL;
				}
				break;

			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			case ZEND_MATCH:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					/* SWITCH variable will be deleted later by FREE, so we can't optimize it */
					Tsource[VAR_NUM(opline->op1.var)] = NULL;
					break;
				}
				if (opline->op1_type == IS_CONST) {
					int target = get_const_switch_target(cfg, op_array, block, opline, &ZEND_OP1_LITERAL(opline));
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					opline->opcode = ZEND_JMP;
					opline->op1_type = IS_UNUSED;
					opline->op2_type = IS_UNUSED;
					block->successors_count = 1;
					block->successors[0] = target;
				}
				break;

			case ZEND_CASE:
			case ZEND_CASE_STRICT:
			case ZEND_COPY_TMP:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					/* Variable will be deleted later by FREE, so we can't optimize it */
					Tsource[VAR_NUM(opline->op1.var)] = NULL;
					break;
				}
				/* break missing intentionally */

			case ZEND_IS_EQUAL:
			case ZEND_IS_NOT_EQUAL:
				if (opline->op1_type == IS_CONST &&
				    opline->op2_type == IS_CONST) {
					goto optimize_constant_binary_op;
				}
		        /* IS_EQ(TRUE, X)      => BOOL(X)
		         * IS_EQ(FALSE, X)     => BOOL_NOT(X)
		         * IS_NOT_EQ(TRUE, X)  => BOOL_NOT(X)
		         * IS_NOT_EQ(FALSE, X) => BOOL(X)
		         * CASE(TRUE, X)       => BOOL(X)
		         * CASE(FALSE, X)      => BOOL_NOT(X)
		         */
				if (opline->op1_type == IS_CONST &&
					(Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_FALSE ||
					 Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_TRUE)) {
					/* Optimization of comparison with "null" is not safe,
					 * because ("0" == null) is not equal to !("0")
					 */
					opline->opcode =
						((opline->opcode != ZEND_IS_NOT_EQUAL) == ((Z_TYPE(ZEND_OP1_LITERAL(opline))) == IS_TRUE)) ?
						ZEND_BOOL : ZEND_BOOL_NOT;
					COPY_NODE(opline->op1, opline->op2);
					SET_UNUSED(opline->op2);
					++(*opt_count);
					goto optimize_bool;
				} else if (opline->op2_type == IS_CONST &&
				           (Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_FALSE ||
				            Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_TRUE)) {
					/* Optimization of comparison with "null" is not safe,
					 * because ("0" == null) is not equal to !("0")
					 */
					opline->opcode =
						((opline->opcode != ZEND_IS_NOT_EQUAL) == ((Z_TYPE(ZEND_OP2_LITERAL(opline))) == IS_TRUE)) ?
						ZEND_BOOL : ZEND_BOOL_NOT;
					SET_UNUSED(opline->op2);
					++(*opt_count);
					goto optimize_bool;
				}
				break;

			case ZEND_BOOL:
			case ZEND_BOOL_NOT:
			optimize_bool:
				if (opline->op1_type == IS_CONST) {
					goto optimize_const_unary_op;
				}
				if (opline->op1_type == IS_TMP_VAR &&
				    !zend_bitset_in(used_ext, VAR_NUM(opline->op1.var))) {
					src = VAR_SOURCE(opline->op1);
					if (src) {
						switch (src->opcode) {
							case ZEND_BOOL_NOT:
								/* T = BOOL_NOT(X) + BOOL(T) -> NOP, BOOL_NOT(X) */
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								opline->opcode = (opline->opcode == ZEND_BOOL) ? ZEND_BOOL_NOT : ZEND_BOOL;
								MAKE_NOP(src);
								++(*opt_count);
								goto optimize_bool;
							case ZEND_BOOL:
								/* T = BOOL(X) + BOOL(T) -> NOP, BOOL(X) */
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								MAKE_NOP(src);
								++(*opt_count);
								goto optimize_bool;
							case ZEND_IS_EQUAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_NOT_EQUAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_IS_NOT_EQUAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_EQUAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_IS_IDENTICAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_NOT_IDENTICAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_IS_NOT_IDENTICAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_IDENTICAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_IS_SMALLER:
								if (opline->opcode == ZEND_BOOL_NOT) {
									zend_uchar tmp_type;
									uint32_t tmp;

									src->opcode = ZEND_IS_SMALLER_OR_EQUAL;
									tmp_type = src->op1_type;
									src->op1_type = src->op2_type;
									src->op2_type = tmp_type;
									tmp = src->op1.num;
									src->op1.num = src->op2.num;
									src->op2.num = tmp;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_IS_SMALLER_OR_EQUAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									zend_uchar tmp_type;
									uint32_t tmp;

									src->opcode = ZEND_IS_SMALLER;
									tmp_type = src->op1_type;
									src->op1_type = src->op2_type;
									src->op2_type = tmp_type;
									tmp = src->op1.num;
									src->op1.num = src->op2.num;
									src->op2.num = tmp;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
							case ZEND_ISSET_ISEMPTY_CV:
							case ZEND_ISSET_ISEMPTY_VAR:
							case ZEND_ISSET_ISEMPTY_DIM_OBJ:
							case ZEND_ISSET_ISEMPTY_PROP_OBJ:
							case ZEND_ISSET_ISEMPTY_STATIC_PROP:
							case ZEND_INSTANCEOF:
							case ZEND_TYPE_CHECK:
							case ZEND_DEFINED:
							case ZEND_IN_ARRAY:
							case ZEND_ARRAY_KEY_EXISTS:
								if (opline->opcode == ZEND_BOOL_NOT) {
									break;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								++(*opt_count);
								break;
						}
					}
				}
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			    while (1) {
					if (opline->op1_type == IS_CONST) {
						++(*opt_count);
						block->successors_count = 1;
						if (zend_is_true(&ZEND_OP1_LITERAL(opline)) ==
						    (opline->opcode == ZEND_JMPZ)) {

							MAKE_NOP(opline);
							block->successors[0] = block->successors[1];
							block->len--;
							cfg->blocks[block->successors[0]].flags |= ZEND_BB_FOLLOW;
							break;
						} else {
							zend_basic_block *next = cfg->blocks + block->successors[1];

							next->flags &= ~ZEND_BB_FOLLOW;
							if (!(next->flags & (ZEND_BB_TARGET|ZEND_BB_PROTECTED))) {
								next->flags &= ~ZEND_BB_REACHABLE;
							}
							opline->opcode = ZEND_JMP;
							COPY_NODE(opline->op1, opline->op2);
							break;
						}
					} else if (opline->op1_type == IS_TMP_VAR &&
					           !zend_bitset_in(used_ext, VAR_NUM(opline->op1.var))) {
						src = VAR_SOURCE(opline->op1);
						if (src) {
							if (src->opcode == ZEND_BOOL_NOT) {
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								/* T = BOOL_NOT(X) + JMPZ(T) -> NOP, JMPNZ(X) */
								opline->opcode = INV_COND(opline->opcode);
								MAKE_NOP(src);
								++(*opt_count);
								continue;
							} else if (src->opcode == ZEND_BOOL ||
							           src->opcode == ZEND_QM_ASSIGN) {
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								MAKE_NOP(src);
								++(*opt_count);
								continue;
							}
						}
					}
					break;
				}
				break;

			case ZEND_JMPZNZ:
				while (1) {
					if (opline->op1_type == IS_CONST) {
						++(*opt_count);
						if (zend_is_true(&ZEND_OP1_LITERAL(opline))) {
							zend_op *target_opline = ZEND_OFFSET_TO_OPLINE(opline, opline->extended_value);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target_opline);
							block->successors[0] = block->successors[1];
						} else {
							zend_op *target_opline = ZEND_OP2_JMP_ADDR(opline);
							ZEND_SET_OP_JMP_ADDR(opline, opline->op1, target_opline);
						}
						block->successors_count = 1;
						opline->op1_type = IS_UNUSED;
						opline->extended_value = 0;
						opline->opcode = ZEND_JMP;
						break;
					} else if (opline->op1_type == IS_TMP_VAR &&
					           !zend_bitset_in(used_ext, VAR_NUM(opline->op1.var))) {
						src = VAR_SOURCE(opline->op1);
						if (src) {
							if (src->opcode == ZEND_BOOL_NOT) {
								/* T = BOOL_NOT(X) + JMPZNZ(T,L1,L2) -> NOP, JMPZNZ(X,L2,L1) */
								uint32_t tmp;

								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								tmp = block->successors[0];
								block->successors[0] = block->successors[1];
								block->successors[1] = tmp;
								MAKE_NOP(src);
								++(*opt_count);
								continue;
							} else if (src->opcode == ZEND_BOOL ||
							           src->opcode == ZEND_QM_ASSIGN) {
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								MAKE_NOP(src);
								++(*opt_count);
								continue;
							}
						}
					}
					break;
				}
				break;

			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
				while (1) {
					if (opline->op1_type == IS_CONST) {
						if (zend_is_true(&ZEND_OP1_LITERAL(opline)) ==
						    (opline->opcode == ZEND_JMPZ_EX)) {

							++(*opt_count);
							opline->opcode = ZEND_QM_ASSIGN;
							zval_ptr_dtor_nogc(&ZEND_OP1_LITERAL(opline));
							ZVAL_BOOL(&ZEND_OP1_LITERAL(opline), opline->opcode == ZEND_JMPZ_EX);
							opline->op2.num = 0;
							block->successors_count = 1;
							block->successors[0] = block->successors[1];
							cfg->blocks[block->successors[0]].flags |= ZEND_BB_FOLLOW;
							break;
						}
					} else if (opline->op1_type == IS_TMP_VAR &&
					           (!zend_bitset_in(used_ext, VAR_NUM(opline->op1.var)) ||
					            opline->result.var == opline->op1.var)) {
						src = VAR_SOURCE(opline->op1);
						if (src) {
							if (src->opcode == ZEND_BOOL ||
							    src->opcode == ZEND_QM_ASSIGN) {
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								MAKE_NOP(src);
								++(*opt_count);
								continue;
							}
						}
					}
					break;
				}
				break;

			case ZEND_CONCAT:
			case ZEND_FAST_CONCAT:
				if (opline->op1_type == IS_CONST &&
				    opline->op2_type == IS_CONST) {
					goto optimize_constant_binary_op;
				}

				if (opline->op2_type == IS_CONST &&
				    opline->op1_type == IS_TMP_VAR) {

					src = VAR_SOURCE(opline->op1);
				    if (src &&
					    (src->opcode == ZEND_CONCAT ||
					     src->opcode == ZEND_FAST_CONCAT) &&
					    src->op2_type == IS_CONST) {
						/* compress consecutive CONCATs */
						int l, old_len;

						if (Z_TYPE(ZEND_OP2_LITERAL(opline)) != IS_STRING) {
							convert_to_string(&ZEND_OP2_LITERAL(opline));
						}
						if (Z_TYPE(ZEND_OP2_LITERAL(src)) != IS_STRING) {
							convert_to_string(&ZEND_OP2_LITERAL(src));
						}

						VAR_SOURCE(opline->op1) = NULL;
						COPY_NODE(opline->op1, src->op1);
						old_len = Z_STRLEN(ZEND_OP2_LITERAL(src));
						l = old_len + Z_STRLEN(ZEND_OP2_LITERAL(opline));
						if (!Z_REFCOUNTED(ZEND_OP2_LITERAL(src))) {
							zend_string *tmp = zend_string_alloc(l, 0);
							memcpy(ZSTR_VAL(tmp), Z_STRVAL(ZEND_OP2_LITERAL(src)), old_len);
							Z_STR(ZEND_OP2_LITERAL(src)) = tmp;
						} else {
							Z_STR(ZEND_OP2_LITERAL(src)) = zend_string_extend(Z_STR(ZEND_OP2_LITERAL(src)), l, 0);
						}
						Z_TYPE_INFO(ZEND_OP2_LITERAL(src)) = IS_STRING_EX;
						memcpy(Z_STRVAL(ZEND_OP2_LITERAL(src)) + old_len, Z_STRVAL(ZEND_OP2_LITERAL(opline)), Z_STRLEN(ZEND_OP2_LITERAL(opline)));
						Z_STRVAL(ZEND_OP2_LITERAL(src))[l] = '\0';
						zval_ptr_dtor_str(&ZEND_OP2_LITERAL(opline));
						ZVAL_STR(&ZEND_OP2_LITERAL(opline), zend_new_interned_string(Z_STR(ZEND_OP2_LITERAL(src))));
						ZVAL_NULL(&ZEND_OP2_LITERAL(src));
						MAKE_NOP(src);
						++(*opt_count);
					}
				}

				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op1);
					if (src &&
					    src->opcode == ZEND_CAST &&
					    src->extended_value == IS_STRING &&
					    src->op1_type != IS_CONST) {
						/* convert T1 = CAST(STRING, X), T2 = CONCAT(T1, Y) to T2 = CONCAT(X,Y) */
						VAR_SOURCE(opline->op1) = NULL;
						COPY_NODE(opline->op1, src->op1);
						MAKE_NOP(src);
						++(*opt_count);
					}
	            }
				if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op2);
					if (src &&
					    src->opcode == ZEND_CAST &&
					    src->extended_value == IS_STRING &&
					    src->op1_type != IS_CONST) {
						/* convert T1 = CAST(STRING, X), T2 = CONCAT(Y, T1) to T2 = CONCAT(Y,X) */
						zend_op *src = VAR_SOURCE(opline->op2);
						VAR_SOURCE(opline->op2) = NULL;
						COPY_NODE(opline->op2, src->op1);
						MAKE_NOP(src);
						++(*opt_count);
					}
				}
				if (opline->op1_type == IS_CONST &&
				    Z_TYPE(ZEND_OP1_LITERAL(opline)) == IS_STRING &&
				    Z_STRLEN(ZEND_OP1_LITERAL(opline)) == 0) {
					/* convert CONCAT('', X) => CAST(STRING, X) */
					literal_dtor(&ZEND_OP1_LITERAL(opline));
					opline->opcode = ZEND_CAST;
					opline->extended_value = IS_STRING;
					COPY_NODE(opline->op1, opline->op2);
					opline->op2_type = IS_UNUSED;
					opline->op2.var = 0;
					++(*opt_count);
				} else if (opline->op2_type == IS_CONST &&
			           Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING &&
			           Z_STRLEN(ZEND_OP2_LITERAL(opline)) == 0) {
					/* convert CONCAT(X, '') => CAST(STRING, X) */
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					opline->opcode = ZEND_CAST;
					opline->extended_value = IS_STRING;
					opline->op2_type = IS_UNUSED;
					opline->op2.var = 0;
					++(*opt_count);
				} else if (opline->opcode == ZEND_CONCAT &&
				           (opline->op1_type == IS_CONST ||
				            (opline->op1_type == IS_TMP_VAR &&
				             VAR_SOURCE(opline->op1) &&
				             (VAR_SOURCE(opline->op1)->opcode == ZEND_FAST_CONCAT ||
				              VAR_SOURCE(opline->op1)->opcode == ZEND_ROPE_END ||
				              VAR_SOURCE(opline->op1)->opcode == ZEND_FETCH_CONSTANT ||
				              VAR_SOURCE(opline->op1)->opcode == ZEND_FETCH_CLASS_CONSTANT))) &&
				           (opline->op2_type == IS_CONST ||
				            (opline->op2_type == IS_TMP_VAR &&
				             VAR_SOURCE(opline->op2) &&
				             (VAR_SOURCE(opline->op2)->opcode == ZEND_FAST_CONCAT ||
				              VAR_SOURCE(opline->op2)->opcode == ZEND_ROPE_END ||
				              VAR_SOURCE(opline->op2)->opcode == ZEND_FETCH_CONSTANT ||
				              VAR_SOURCE(opline->op2)->opcode == ZEND_FETCH_CLASS_CONSTANT)))) {
					opline->opcode = ZEND_FAST_CONCAT;
					++(*opt_count);
				}
				break;

			case ZEND_ADD:
			case ZEND_SUB:
			case ZEND_MUL:
			case ZEND_DIV:
			case ZEND_MOD:
			case ZEND_SL:
			case ZEND_SR:
			case ZEND_IS_SMALLER:
			case ZEND_IS_SMALLER_OR_EQUAL:
			case ZEND_IS_IDENTICAL:
			case ZEND_IS_NOT_IDENTICAL:
			case ZEND_BOOL_XOR:
			case ZEND_BW_OR:
			case ZEND_BW_AND:
			case ZEND_BW_XOR:
				if (opline->op1_type == IS_CONST &&
				    opline->op2_type == IS_CONST) {
					/* evaluate constant expressions */
					zval result;

optimize_constant_binary_op:
					if (zend_optimizer_eval_binary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline)) == SUCCESS) {
						literal_dtor(&ZEND_OP1_LITERAL(opline));
						literal_dtor(&ZEND_OP2_LITERAL(opline));
						opline->opcode = ZEND_QM_ASSIGN;
						SET_UNUSED(opline->op2);
						zend_optimizer_update_op1_const(op_array, opline, &result);
						++(*opt_count);
					}
				}
				break;

			case ZEND_BW_NOT:
				if (opline->op1_type == IS_CONST) {
					/* evaluate constant unary ops */
					zval result;

optimize_const_unary_op:
					if (zend_optimizer_eval_unary_op(&result, opline->opcode, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
						literal_dtor(&ZEND_OP1_LITERAL(opline));
						opline->opcode = ZEND_QM_ASSIGN;
						zend_optimizer_update_op1_const(op_array, opline, &result);
						++(*opt_count);
					}
				}
				break;

			case ZEND_CAST:
				if (opline->op1_type == IS_CONST) {
					/* cast of constant operand */
					zval result;

					if (zend_optimizer_eval_cast(&result, opline->extended_value, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
						literal_dtor(&ZEND_OP1_LITERAL(opline));
						opline->opcode = ZEND_QM_ASSIGN;
						opline->extended_value = 0;
						zend_optimizer_update_op1_const(op_array, opline, &result);
						++(*opt_count);
					}
				}
				break;

			case ZEND_STRLEN:
				if (opline->op1_type == IS_CONST) {
					zval result;

					if (zend_optimizer_eval_strlen(&result, &ZEND_OP1_LITERAL(opline)) == SUCCESS) {
						literal_dtor(&ZEND_OP1_LITERAL(opline));
						opline->opcode = ZEND_QM_ASSIGN;
						zend_optimizer_update_op1_const(op_array, opline, &result);
						++(*opt_count);
					}
				}
				break;

			case ZEND_RETURN:
			case ZEND_EXIT:
				if (opline->op1_type == IS_TMP_VAR) {
					src = VAR_SOURCE(opline->op1);
					if (src && src->opcode == ZEND_QM_ASSIGN) {
						zend_op *op = src + 1;
						zend_bool optimize = 1;

						while (op < opline) {
							if ((op->op1_type == opline->op1_type
							  && op->op1.var == opline->op1.var)
							 || (op->op2_type == opline->op1_type
							  && op->op2.var == opline->op1.var)) {
								optimize = 0;
								break;
							}
							op++;
						}

						if (optimize) {
							/* T = QM_ASSIGN(X), RETURN(T) to NOP, RETURN(X) */
							VAR_SOURCE(opline->op1) = NULL;
							COPY_NODE(opline->op1, src->op1);
							MAKE_NOP(src);
							++(*opt_count);
						}
					}
				}
				break;

			case ZEND_QM_ASSIGN:
				if (opline->op1_type == opline->result_type &&
				    opline->op1.var == opline->result.var) {
					/* strip T = QM_ASSIGN(T) */
					MAKE_NOP(opline);
					++(*opt_count);
				} else if (opline->op1_type == IS_TMP_VAR &&
				           opline->result_type == IS_TMP_VAR &&
				           !zend_bitset_in(used_ext, VAR_NUM(opline->op1.var))) {
					/* T1 = ..., T2 = QM_ASSIGN(T1) to T2 = ..., NOP */
					src = VAR_SOURCE(opline->op1);
					if (src &&
						src->opcode != ZEND_COPY_TMP &&
						src->opcode != ZEND_ADD_ARRAY_ELEMENT &&
						src->opcode != ZEND_ADD_ARRAY_UNPACK &&
						(src->opcode != ZEND_DECLARE_LAMBDA_FUNCTION ||
						 src == opline -1)) {
						src->result.var = opline->result.var;
						VAR_SOURCE(opline->op1) = NULL;
						VAR_SOURCE(opline->result) = src;
						MAKE_NOP(opline);
						++(*opt_count);
					}
				}
				break;
		}

		/* get variable source */
		if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
			SET_VAR_SOURCE(opline);
		}
		opline++;
	}
}

/* Rebuild plain (optimized) op_array from CFG */
static void assemble_code_blocks(zend_cfg *cfg, zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_basic_block *blocks = cfg->blocks;
	zend_basic_block *end = blocks + cfg->blocks_count;
	zend_basic_block *b;
	zend_op *new_opcodes;
	zend_op *opline;
	uint32_t len = 0;
	int n;

	for (b = blocks; b < end; b++) {
		if (b->len == 0) {
			continue;
		}
		if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			if (b->flags & ZEND_BB_UNREACHABLE_FREE) {
				/* Only keep the FREE for the loop var */
				ZEND_ASSERT(op_array->opcodes[b->start].opcode == ZEND_FREE
						|| op_array->opcodes[b->start].opcode == ZEND_FE_FREE);
				len += b->len = 1;
				continue;
			}

			opline = op_array->opcodes + b->start + b->len - 1;
			if (opline->opcode == ZEND_JMP) {
				zend_basic_block *next = b + 1;

				while (next < end && !(next->flags & ZEND_BB_REACHABLE)) {
					next++;
				}
				if (next < end && next == blocks + b->successors[0]) {
					/* JMP to the next block - strip it */
					MAKE_NOP(opline);
					b->len--;
				}
			} else if (b->len == 1 && opline->opcode == ZEND_NOP) {
				/* skip empty block */
				b->len--;
			}
			len += b->len;
		} else {
			/* this block will not be used, delete all constants there */
			zend_op *op = op_array->opcodes + b->start;
			zend_op *end = op + b->len;
			for (; op < end; op++) {
				if (op->op1_type == IS_CONST) {
					literal_dtor(&ZEND_OP1_LITERAL(op));
				}
				if (op->op2_type == IS_CONST) {
					literal_dtor(&ZEND_OP2_LITERAL(op));
				}
			}
		}
	}

	new_opcodes = emalloc(len * sizeof(zend_op));
	opline = new_opcodes;

	/* Copy code of reachable blocks into a single buffer */
	for (b = blocks; b < end; b++) {
		if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			memcpy(opline, op_array->opcodes + b->start, b->len * sizeof(zend_op));
			b->start = opline - new_opcodes;
			opline += b->len;
		}
	}

	/* adjust jump targets */
	efree(op_array->opcodes);
	op_array->opcodes = new_opcodes;
	op_array->last = len;

	for (b = blocks; b < end; b++) {
		if (!(b->flags & ZEND_BB_REACHABLE) || b->len == 0) {
			continue;
		}
		opline = op_array->opcodes + b->start + b->len - 1;
		switch (opline->opcode) {
			case ZEND_FAST_CALL:
			case ZEND_JMP:
				ZEND_SET_OP_JMP_ADDR(opline, opline->op1, new_opcodes + blocks[b->successors[0]].start);
				break;
			case ZEND_JMPZNZ:
				opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[1]].start);
				/* break missing intentionally */
			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_FE_RESET_R:
			case ZEND_FE_RESET_RW:
			case ZEND_JMP_SET:
			case ZEND_COALESCE:
			case ZEND_ASSERT_CHECK:
			case ZEND_JMP_NULL:
				ZEND_SET_OP_JMP_ADDR(opline, opline->op2, new_opcodes + blocks[b->successors[0]].start);
				break;
			case ZEND_CATCH:
				if (!(opline->extended_value & ZEND_LAST_CATCH)) {
					ZEND_SET_OP_JMP_ADDR(opline, opline->op2, new_opcodes + blocks[b->successors[0]].start);
				}
				break;
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[0]].start);
				break;
			case ZEND_SWITCH_LONG:
			case ZEND_SWITCH_STRING:
			case ZEND_MATCH:
			{
				HashTable *jumptable = Z_ARRVAL(ZEND_OP2_LITERAL(opline));
				zval *zv;
				uint32_t s = 0;
				ZEND_ASSERT(b->successors_count == (opline->opcode == ZEND_MATCH ? 1 : 2) + zend_hash_num_elements(jumptable));

				ZEND_HASH_FOREACH_VAL(jumptable, zv) {
					Z_LVAL_P(zv) = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[s++]].start);
				} ZEND_HASH_FOREACH_END();
				opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[s++]].start);
				break;
			}
		}
	}

	/* adjust exception jump targets & remove unused try_catch_array entries */
	if (op_array->last_try_catch) {
		int i, j;
		uint32_t *map;
		ALLOCA_FLAG(use_heap);

		map = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last_try_catch, use_heap);
		for (i = 0, j = 0; i< op_array->last_try_catch; i++) {
			if (blocks[cfg->map[op_array->try_catch_array[i].try_op]].flags & ZEND_BB_REACHABLE) {
				map[i] = j;
				op_array->try_catch_array[j].try_op = blocks[cfg->map[op_array->try_catch_array[i].try_op]].start;
				if (op_array->try_catch_array[i].catch_op) {
					op_array->try_catch_array[j].catch_op = blocks[cfg->map[op_array->try_catch_array[i].catch_op]].start;
				} else {
					op_array->try_catch_array[j].catch_op =  0;
				}
				if (op_array->try_catch_array[i].finally_op) {
					op_array->try_catch_array[j].finally_op = blocks[cfg->map[op_array->try_catch_array[i].finally_op]].start;
				} else {
					op_array->try_catch_array[j].finally_op =  0;
				}
				if (!op_array->try_catch_array[i].finally_end) {
					op_array->try_catch_array[j].finally_end = 0;
				} else {
					op_array->try_catch_array[j].finally_end = blocks[cfg->map[op_array->try_catch_array[i].finally_end]].start;
				}
				j++;
			}
		}
		if (i != j) {
			op_array->last_try_catch = j;
			if (j == 0) {
				efree(op_array->try_catch_array);
				op_array->try_catch_array = NULL;
			}

			if (op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK) {
				zend_op *opline = new_opcodes;
				zend_op *end = opline + len;
				while (opline < end) {
					if (opline->opcode == ZEND_FAST_RET &&
					    opline->op2.num != (uint32_t)-1 &&
					    opline->op2.num < (uint32_t)j) {
						opline->op2.num = map[opline->op2.num];
					}
					opline++;
				}
			}
		}
		free_alloca(map, use_heap);
	}

	/* adjust early binding list */
	if (op_array->fn_flags & ZEND_ACC_EARLY_BINDING) {
		ZEND_ASSERT(op_array == &ctx->script->main_op_array);
		ctx->script->first_early_binding_opline =
			zend_build_delayed_early_binding_list(op_array);
	}

	/* rebuild map (just for printing) */
	memset(cfg->map, -1, sizeof(int) * op_array->last);
	for (n = 0; n < cfg->blocks_count; n++) {
		if (cfg->blocks[n].flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
			cfg->map[cfg->blocks[n].start] = n;
		}
	}
}

static zend_always_inline zend_basic_block *get_target_block(const zend_cfg *cfg, zend_basic_block *block, int n, uint32_t *opt_count)
{
	int b;
	zend_basic_block *target_block = cfg->blocks + block->successors[n];

	if (target_block->len == 0 && !(target_block->flags & ZEND_BB_PROTECTED)) {
		do {
			b = target_block->successors[0];
			target_block = cfg->blocks + b;
		} while (target_block->len == 0 && !(target_block->flags & ZEND_BB_PROTECTED));
		block->successors[n] = b;
		++(*opt_count);
	}
	return target_block;
}

static zend_always_inline zend_basic_block *get_follow_block(const zend_cfg *cfg, zend_basic_block *block, int n, uint32_t *opt_count)
{
	int b;
	zend_basic_block *target_block = cfg->blocks + block->successors[n];

	if (target_block->len == 0 && !(target_block->flags & ZEND_BB_PROTECTED)) {
		do {
			b = target_block->successors[0];
			target_block = cfg->blocks + b;
		} while (target_block->len == 0 && !(target_block->flags & ZEND_BB_PROTECTED));
		block->successors[n] = b;
		++(*opt_count);
	}
	return target_block;
}

static zend_always_inline zend_basic_block *get_next_block(const zend_cfg *cfg, zend_basic_block *block)
{
	zend_basic_block *next_block = block + 1;
	zend_basic_block *end = cfg->blocks + cfg->blocks_count;

	while (1) {
		if (next_block == end) {
			return NULL;
		} else if (next_block->flags & ZEND_BB_REACHABLE) {
			break;
		}
		next_block++;
	}
	while (next_block->len == 0 && !(next_block->flags & ZEND_BB_PROTECTED)) {
		next_block = cfg->blocks + next_block->successors[0];
	}
	return next_block;
}


/* we use "jmp_hitlist" to avoid infinity loops during jmp optimization */
static zend_always_inline int in_hitlist(int target, int *jmp_hitlist, int jmp_hitlist_count)
{
	int i;

	for (i = 0; i < jmp_hitlist_count; i++) {
		if (jmp_hitlist[i] == target) {
			return 1;
		}
	}
	return 0;
}

#define CHECK_LOOP(target) \
	if (EXPECTED(!in_hitlist(target, jmp_hitlist, jmp_hitlist_count))) { \
		jmp_hitlist[jmp_hitlist_count++] = target;	\
	} else { \
		break; \
	}

static void zend_jmp_optimization(zend_basic_block *block, zend_op_array *op_array, const zend_cfg *cfg, int *jmp_hitlist, uint32_t *opt_count)
{
	/* last_op is the last opcode of the current block */
	zend_basic_block *target_block, *follow_block, *next_block;
	zend_op *last_op, *target;
	int next, jmp_hitlist_count;

	if (block->len == 0) {
		return;
	}

	last_op = op_array->opcodes + block->start + block->len - 1;
	switch (last_op->opcode) {
		case ZEND_JMP:
			jmp_hitlist_count = 0;

			target_block = get_target_block(cfg, block, 0, opt_count);
			while (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;
				if (target->opcode == ZEND_JMP) {
					/* JMP L, L: JMP L1 -> JMP L1 */
					next = target_block->successors[0];
				} else {
					break;
				}
				CHECK_LOOP(next);
				block->successors[0] = next;
				++(*opt_count);
				target_block = get_target_block(cfg, block, 0, opt_count);
			}

			next_block = get_next_block(cfg, block);
			if (target_block == next_block) {
				/* JMP(next) -> NOP */
				MAKE_NOP(last_op);
				++(*opt_count);
				block->len--;
			} else if (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;
				if (target->opcode == ZEND_JMPZNZ) {
					/* JMP L, L: JMPZNZ L1,L2 -> JMPZNZ L1,L2 */
					*last_op = *target;
					if (last_op->op1_type == IS_CONST) {
						zval zv;
						ZVAL_COPY(&zv, &ZEND_OP1_LITERAL(last_op));
						last_op->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
					block->successors_count = 2;
					block->successors[0] = target_block->successors[0];
					block->successors[1] = target_block->successors[1];
					++(*opt_count);
					goto optimize_jmpznz;
				} else if ((target->opcode == ZEND_RETURN ||
				            target->opcode == ZEND_RETURN_BY_REF ||
				            target->opcode == ZEND_GENERATOR_RETURN ||
				            target->opcode == ZEND_EXIT) &&
				           !(op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)) {
					/* JMP L, L: RETURN to immediate RETURN */
					*last_op = *target;
					if (last_op->op1_type == IS_CONST) {
						zval zv;
						ZVAL_COPY(&zv, &ZEND_OP1_LITERAL(last_op));
						last_op->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
					block->successors_count = 0;
					++(*opt_count);
				}
			}
			break;

		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_JMP_NULL:
			jmp_hitlist_count = 0;

			target_block = get_target_block(cfg, block, 0, opt_count);
			while (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;

				if (target->opcode == ZEND_JMP) {
					/* JMP_SET(X, L), L: JMP(L2) -> JMP_SET(X, L2) */
					next = target_block->successors[0];
					CHECK_LOOP(next);
					block->successors[0] = next;
					++(*opt_count);
				} else {
					break;
				}
				target_block = get_target_block(cfg, block, 0, opt_count);
			}
			break;

		case ZEND_JMPZ:
		case ZEND_JMPNZ:
			jmp_hitlist_count = 0;

			target_block = get_target_block(cfg, block, 0, opt_count);
			while (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;

				if (target->opcode == ZEND_JMP) {
					/* JMPZ(X, L), L: JMP(L2) -> JMPZ(X, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == last_op->opcode &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZ(X, L), L: JMPZ(X, L2) -> JMPZ(X, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == INV_COND(last_op->opcode) &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZ(X, L), L: JMPNZ(X, L2) -> JMPZ(X, L+1) */
					next = target_block->successors[1];
				} else if (target->opcode == ZEND_JMPZNZ &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZ(X, L), L: JMPZNZ(X, L2, L3) -> JMPZ(X, L2) */
					next = target_block->successors[last_op->opcode == ZEND_JMPNZ];
				} else {
					break;
				}
				CHECK_LOOP(next);
				block->successors[0] = next;
				++(*opt_count);
				target_block = get_target_block(cfg, block, 0, opt_count);
			}

			follow_block = get_follow_block(cfg, block, 1, opt_count);
			if (target_block == follow_block) {
				/* L: JMP[N]Z(X, L+1) -> NOP or FREE(X) */
				if (last_op->op1_type == IS_CV) {
					last_op->opcode = ZEND_CHECK_VAR;
					last_op->op2.num = 0;
				} else if (last_op->op1_type & (IS_VAR|IS_TMP_VAR)) {
					last_op->opcode = ZEND_FREE;
					last_op->op2.num = 0;
				} else {
					MAKE_NOP(last_op);
					block->len--;
				}
				block->successors_count = 1;
				++(*opt_count);
			} else if (follow_block->len == 1) {
				target = op_array->opcodes + follow_block->start;
				if (target->opcode == ZEND_JMP) {
				    if (block->successors[0] == follow_block->successors[0]) {
						/* JMPZ(X,L1), JMP(L1) -> NOP, JMP(L1) */
						if (last_op->op1_type == IS_CV) {
							last_op->opcode = ZEND_CHECK_VAR;
							last_op->op2.num = 0;
						} else if (last_op->op1_type & (IS_VAR|IS_TMP_VAR)) {
							last_op->opcode = ZEND_FREE;
							last_op->op2.num = 0;
						} else {
							MAKE_NOP(last_op);
							block->len--;
						}
						block->successors[0] = follow_block - cfg->blocks;
						block->successors_count = 1;
						++(*opt_count);
						break;
					} else if (!(follow_block->flags & (ZEND_BB_TARGET | ZEND_BB_PROTECTED))) {
						next_block = get_next_block(cfg, follow_block);

						if (target_block == next_block) {
							/* JMPZ(X,L1) JMP(L2) L1: -> JMPNZ(X,L2) NOP*/

							last_op->opcode = INV_COND(last_op->opcode);

							block->successors[0] = follow_block->successors[0];
							block->successors[1] = next_block - cfg->blocks;

							follow_block->flags &= ~ZEND_BB_REACHABLE;
							MAKE_NOP(target);
							follow_block->len = 0;

							next_block->flags |= ZEND_BB_FOLLOW;

							break;
						}
					}

					/* JMPZ(X,L1), JMP(L2) -> JMPZNZ(X,L1,L2) */
					if (last_op->opcode == ZEND_JMPZ) {
						block->successors[1] = follow_block->successors[0];
					} else {
						block->successors[1] = block->successors[0];
						block->successors[0] = follow_block->successors[0];
					}
					last_op->opcode = ZEND_JMPZNZ;
					++(*opt_count);
				}
			}
			break;

		case ZEND_JMPNZ_EX:
		case ZEND_JMPZ_EX:
			jmp_hitlist_count = 0;

			target_block = get_target_block(cfg, block, 0, opt_count);
			while (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;

				if (target->opcode == ZEND_JMP) {
					/* T = JMPZ_EX(X, L), L: JMP(L2) -> T = JMPZ(X, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == last_op->opcode-3 &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* T = JMPZ_EX(X, L1), L1: JMPZ({X|T}, L2) -> T = JMPZ_EX(X, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == last_op->opcode &&
				           target->result.var == last_op->result.var &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* T = JMPZ_EX(X, L1), L1: T = JMPZ_EX({X|T}, L2) -> T = JMPZ_EX(X, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == ZEND_JMPZNZ &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* T = JMPZ_EX(X, L), L: JMPZNZ({X|T}, L2, L3) -> T = JMPZ_EX(X, L2) */
					next = target_block->successors[last_op->opcode == ZEND_JMPNZ_EX];
				} else if (target->opcode == INV_EX_COND(last_op->opcode) &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* T = JMPZ_EX(X, L1), L1: JMPNZ({X|T1}, L2) -> T = JMPZ_EX(X, L1+1) */
					next = target_block->successors[1];
				} else if (target->opcode == INV_EX_COND_EX(last_op->opcode) &&
				           target->result.var == last_op->result.var &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* T = JMPZ_EX(X, L1), L1: T = JMPNZ_EX({X|T}, L2) -> T = JMPZ_EX(X, L1+1) */
					next = target_block->successors[1];
				} else if (target->opcode == ZEND_BOOL &&
				           (SAME_VAR(target->op1, last_op->result) ||
				            SAME_VAR(target->op1, last_op->op1))) {
					/* convert Y = JMPZ_EX(X,L1), L1: Z = BOOL(Y) to
					   Z = JMPZ_EX(X,L1+1) */

					/* NOTE: This optimization pattern is not safe, but works, */
					/*       because result of JMPZ_EX instruction             */
					/*       is not used on the following path and             */
					/*       should be used once on the branch path.           */
					/*                                                         */
					/*       The pattern works well only if jums processed in  */
					/*       direct order, otherwise it breaks JMPZ_EX         */
					/*       sequences too early.                              */
					last_op->result.var = target->result.var;
					next = target_block->successors[0];
				} else {
					break;
				}
				CHECK_LOOP(next);
				block->successors[0] = next;
				++(*opt_count);
				target_block = get_target_block(cfg, block, 0, opt_count);
			}

			follow_block = get_follow_block(cfg, block, 1, opt_count);
			if (target_block == follow_block) {
				/* L: T = JMP[N]Z_EX(X, L+1) -> T = BOOL(X) */
				last_op->opcode = ZEND_BOOL;
				last_op->op2.num = 0;
				block->successors_count = 1;
				++(*opt_count);
				break;
			}
			break;

		case ZEND_JMPZNZ: {
optimize_jmpznz:
			jmp_hitlist_count = 0;
			target_block = get_target_block(cfg, block, 0, opt_count);
			while (target_block->len == 1) {
				target = op_array->opcodes + target_block->start;

				if (target->opcode == ZEND_JMP) {
					/* JMPZNZ(X, L1, L2), L1: JMP(L3) -> JMPZNZ(X, L3, L2) */
					next = target_block->successors[0];
				} else if ((target->opcode == ZEND_JMPZ || target->opcode == ZEND_JMPZNZ) &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZNZ(X, L1, L2), L1: JMPZ(X, L3) -> JMPZNZ(X, L3, L2) */
					next = target_block->successors[0];
				} else if (target->opcode == ZEND_JMPNZ &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZNZ(X, L1, L2), L1: X = JMPNZ(X, L3) -> JMPZNZ(X, L1+1, L2) */
					next = target_block->successors[1];
				} else {
					break;
				}
				CHECK_LOOP(next);
				block->successors[0] = next;
				++(*opt_count);
				target_block = get_target_block(cfg, block, 0, opt_count);
			}

			jmp_hitlist_count = 0;
			follow_block = get_target_block(cfg, block, 1, opt_count);
			while (follow_block->len == 1) {
				target = op_array->opcodes + follow_block->start;

				if (target->opcode == ZEND_JMP) {
					/* JMPZNZ(X, L1, L2), L2: JMP(L3) -> JMPZNZ(X, L1, L3) */
					next = follow_block->successors[0];
				} else if (target->opcode == ZEND_JMPNZ &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZNZ(X, L1, L2), L2: X = JMPNZ(X, L3) -> JMPZNZ(X, L1, L3) */
					next = follow_block->successors[0];
				} else if ((target->opcode == ZEND_JMPZ || target->opcode == ZEND_JMPZNZ) &&
				           SAME_VAR(target->op1, last_op->op1)) {
					/* JMPZNZ(X, L1, L2), L2: JMPZ(X, L3) -> JMPZNZ(X, L1, L2+1) */
					next = follow_block->successors[1];
				} else {
					break;
				}
				CHECK_LOOP(next);
				block->successors[1] = next;
				++(*opt_count);
				follow_block = get_target_block(cfg, block, 1, opt_count);
			}

			next_block = get_next_block(cfg, block);
			if (target_block == follow_block &&
			    !(last_op->op1_type & (IS_VAR|IS_TMP_VAR))) {
				/* JMPZNZ(?,L,L) -> JMP(L) */
				last_op->opcode = ZEND_JMP;
				SET_UNUSED(last_op->op1);
				SET_UNUSED(last_op->op2);
				last_op->extended_value = 0;
				block->successors_count = 1;
				++(*opt_count);
			} else if (target_block == next_block) {
				/* jumping to next on Z - can follow to it and jump only on NZ */
				/* JMPZNZ(X,L1,L2) L1: -> JMPNZ(X,L2) */
				int tmp = block->successors[0];
				last_op->opcode = ZEND_JMPNZ;
				block->successors[0] = block->successors[1];
				block->successors[1] = tmp;
				++(*opt_count);
			} else if (follow_block == next_block) {
				/* jumping to next on NZ - can follow to it and jump only on Z */
				/* JMPZNZ(X,L1,L2) L2: -> JMPZ(X,L1) */
				last_op->opcode = ZEND_JMPZ;
				++(*opt_count);
			}
			break;
		}
	}
}

/* Global data dependencies */

/* Find a set of variables which are used outside of the block where they are
 * defined. We won't apply some optimization patterns for such variables. */
static void zend_t_usage(zend_cfg *cfg, zend_op_array *op_array, zend_bitset used_ext, zend_optimizer_ctx *ctx)
{
	int n;
	zend_basic_block *block, *next_block;
	uint32_t var_num;
	uint32_t bitset_len;
	zend_bitset usage;
	zend_bitset defined_here;
	void *checkpoint;
	zend_op *opline, *end;


	if (op_array->T == 0) {
		/* shortcut - if no Ts, nothing to do */
		return;
	}

	checkpoint = zend_arena_checkpoint(ctx->arena);
	bitset_len = zend_bitset_len(op_array->last_var + op_array->T);
	defined_here = zend_arena_alloc(&ctx->arena, bitset_len * ZEND_BITSET_ELM_SIZE);

	zend_bitset_clear(defined_here, bitset_len);
	for (n = 1; n < cfg->blocks_count; n++) {
		block = cfg->blocks + n;

		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		opline = op_array->opcodes + block->start;
		end = opline + block->len;
		if (!(block->flags & ZEND_BB_FOLLOW) ||
		    (block->flags & ZEND_BB_TARGET)) {
			/* Skip continuation of "extended" BB */
			zend_bitset_clear(defined_here, bitset_len);
		}

		while (opline<end) {
			if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
				var_num = VAR_NUM(opline->op1.var);
				if (!zend_bitset_in(defined_here, var_num)) {
					zend_bitset_incl(used_ext, var_num);
				}
			}
			if (opline->op2_type == IS_VAR) {
				var_num = VAR_NUM(opline->op2.var);
				if (opline->opcode == ZEND_FE_FETCH_R ||
				    opline->opcode == ZEND_FE_FETCH_RW) {
					/* these opcode use the op2 as result */
					zend_bitset_incl(defined_here, var_num);
				} else if (!zend_bitset_in(defined_here, var_num)) {
					zend_bitset_incl(used_ext, var_num);
				}
			} else if (opline->op2_type == IS_TMP_VAR) {
				var_num = VAR_NUM(opline->op2.var);
				if (!zend_bitset_in(defined_here, var_num)) {
					zend_bitset_incl(used_ext, var_num);
				}
			}

			if (opline->result_type == IS_VAR) {
				var_num = VAR_NUM(opline->result.var);
				zend_bitset_incl(defined_here, var_num);
			} else if (opline->result_type == IS_TMP_VAR) {
				var_num = VAR_NUM(opline->result.var);
				switch (opline->opcode) {
					case ZEND_ADD_ARRAY_ELEMENT:
					case ZEND_ADD_ARRAY_UNPACK:
					case ZEND_ROPE_ADD:
						/* these opcodes use the result as argument */
						if (!zend_bitset_in(defined_here, var_num)) {
							zend_bitset_incl(used_ext, var_num);
						}
						break;
					default :
						zend_bitset_incl(defined_here, var_num);
				}
			}
			opline++;
		}
	}

	if (ctx->debug_level & ZEND_DUMP_BLOCK_PASS_VARS) {
		int printed = 0;
		uint32_t i;

		for (i = op_array->last_var; i< op_array->T; i++) {
			if (zend_bitset_in(used_ext, i)) {
				if (!printed) {
					fprintf(stderr, "NON-LOCAL-VARS: %d", i);
					printed = 1;
				} else {
					fprintf(stderr, ", %d", i);
				}
			}
		}
		if (printed) {
			fprintf(stderr, "\n");
		}
	}

	usage = defined_here;
	next_block = NULL;
	for (n = cfg->blocks_count; n > 0;) {
		block = cfg->blocks + (--n);

		if (!(block->flags & ZEND_BB_REACHABLE) || block->len == 0) {
			continue;
		}

		end = op_array->opcodes + block->start;
		opline = end + block->len - 1;
		if (!next_block ||
		    !(next_block->flags & ZEND_BB_FOLLOW) ||
		    (next_block->flags & ZEND_BB_TARGET)) {
			/* Skip continuation of "extended" BB */
			zend_bitset_copy(usage, used_ext, bitset_len);
		} else if (block->successors_count > 1) {
			zend_bitset_union(usage, used_ext, bitset_len);
		}
		next_block = block;

		while (opline >= end) {
			/* usage checks */
			if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
				if (!zend_bitset_in(usage, VAR_NUM(opline->result.var))) {
					switch (opline->opcode) {
						case ZEND_ASSIGN_OP:
						case ZEND_ASSIGN_DIM_OP:
						case ZEND_ASSIGN_OBJ_OP:
						case ZEND_ASSIGN_STATIC_PROP_OP:
						case ZEND_PRE_INC:
						case ZEND_PRE_DEC:
						case ZEND_ASSIGN:
						case ZEND_ASSIGN_REF:
						case ZEND_DO_FCALL:
						case ZEND_DO_ICALL:
						case ZEND_DO_UCALL:
						case ZEND_DO_FCALL_BY_NAME:
							opline->result_type = IS_UNUSED;
							break;
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
						case ZEND_POST_INC_OBJ:
						case ZEND_POST_DEC_OBJ:
						case ZEND_POST_INC_STATIC_PROP:
						case ZEND_POST_DEC_STATIC_PROP:
							opline->opcode -= 2;
							opline->result_type = IS_UNUSED;
							break;
						case ZEND_QM_ASSIGN:
						case ZEND_BOOL:
						case ZEND_BOOL_NOT:
							if (opline->op1_type == IS_CV) {
								opline->opcode = ZEND_CHECK_VAR;
								SET_UNUSED(opline->result);
							} else if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
								opline->opcode = ZEND_FREE;
								SET_UNUSED(opline->result);
							} else {
								if (opline->op1_type == IS_CONST) {
									literal_dtor(&ZEND_OP1_LITERAL(opline));
								}
								MAKE_NOP(opline);
							}
							break;
						case ZEND_JMPZ_EX:
						case ZEND_JMPNZ_EX:
							opline->opcode -= 3;
							SET_UNUSED(opline->result);
							break;
						case ZEND_ADD_ARRAY_ELEMENT:
						case ZEND_ADD_ARRAY_UNPACK:
						case ZEND_ROPE_ADD:
							zend_bitset_incl(usage, VAR_NUM(opline->result.var));
							break;
					}
				} else {
					switch (opline->opcode) {
						case ZEND_ADD_ARRAY_ELEMENT:
						case ZEND_ADD_ARRAY_UNPACK:
						case ZEND_ROPE_ADD:
							break;
						default:
							zend_bitset_excl(usage, VAR_NUM(opline->result.var));
							break;
					}
				}
			}

			if (opline->op2_type == IS_VAR) {
				switch (opline->opcode) {
					case ZEND_FE_FETCH_R:
					case ZEND_FE_FETCH_RW:
						zend_bitset_excl(usage, VAR_NUM(opline->op2.var));
						break;
					default:
						zend_bitset_incl(usage, VAR_NUM(opline->op2.var));
						break;
				}
			} else if (opline->op2_type == IS_TMP_VAR) {
				zend_bitset_incl(usage, VAR_NUM(opline->op2.var));
			}

			if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
				zend_bitset_incl(usage, VAR_NUM(opline->op1.var));
			}

			opline--;
		}
	}

	zend_arena_release(&ctx->arena, checkpoint);
}

static void zend_merge_blocks(zend_op_array *op_array, zend_cfg *cfg, uint32_t *opt_count)
{
	int i;
	zend_basic_block *b, *bb;
	zend_basic_block *prev = NULL;

	for (i = 0; i < cfg->blocks_count; i++) {
		b = cfg->blocks + i;
		if (b->flags & ZEND_BB_REACHABLE) {
			if ((b->flags & ZEND_BB_FOLLOW) &&
			    !(b->flags & (ZEND_BB_TARGET | ZEND_BB_PROTECTED)) &&
			    prev && prev->successors_count == 1 && prev->successors[0] == i)
			{
				zend_op *last_op = op_array->opcodes + prev->start + prev->len - 1;
				if (prev->len != 0 && last_op->opcode == ZEND_JMP) {
					MAKE_NOP(last_op);
				}

				for (bb = prev + 1; bb != b; bb++) {
					zend_op *op = op_array->opcodes + bb->start;
					zend_op *end = op + bb->len;
					while (op < end) {
						if (op->op1_type == IS_CONST) {
							literal_dtor(&ZEND_OP1_LITERAL(op));
						}
						if (op->op2_type == IS_CONST) {
							literal_dtor(&ZEND_OP2_LITERAL(op));
						}
						MAKE_NOP(op);
						op++;
					}
					/* make block empty */
					bb->len = 0;
				}

				/* re-link */
				prev->flags |= (b->flags & ZEND_BB_EXIT);
				prev->len = b->start + b->len - prev->start;
				prev->successors_count = b->successors_count;
				if (b->successors != b->successors_storage) {
					prev->successors = b->successors;
					b->successors = b->successors_storage;
				} else {
					memcpy(prev->successors, b->successors, b->successors_count * sizeof(int));
				}

				/* unlink & make block empty and unreachable */
				b->flags = 0;
				b->len = 0;
				b->successors_count = 0;
				++(*opt_count);
			} else {
				prev = b;
			}
		}
	}
}

#define PASSES 3

void zend_optimize_cfg(zend_op_array *op_array, zend_optimizer_ctx *ctx)
{
	zend_cfg cfg;
	zend_basic_block *blocks, *end, *b;
	int pass;
	uint32_t bitset_len;
	zend_bitset usage;
	void *checkpoint;
	zend_op **Tsource;
	uint32_t opt_count;
	int *jmp_hitlist;

    /* Build CFG */
	checkpoint = zend_arena_checkpoint(ctx->arena);
	if (zend_build_cfg(&ctx->arena, op_array, 0, &cfg) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (cfg.blocks_count * (op_array->last_var + op_array->T) > 64 * 1024 * 1024) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_BLOCK_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG, "before block pass", &cfg);
	}

	bitset_len = zend_bitset_len(op_array->last_var + op_array->T);
	Tsource = zend_arena_calloc(&ctx->arena, op_array->last_var + op_array->T, sizeof(zend_op *));
	usage = zend_arena_alloc(&ctx->arena, bitset_len * ZEND_BITSET_ELM_SIZE);
	jmp_hitlist = zend_arena_alloc(&ctx->arena, cfg.blocks_count * sizeof(int));

	blocks = cfg.blocks;
	end = blocks + cfg.blocks_count;
	for (pass = 0; pass < PASSES; pass++) {
		opt_count = 0;

		/* Compute data dependencies */
		zend_bitset_clear(usage, bitset_len);
		zend_t_usage(&cfg, op_array, usage, ctx);

		/* optimize each basic block separately */
		for (b = blocks; b < end; b++) {
			if (!(b->flags & ZEND_BB_REACHABLE)) {
				continue;
			}
			/* we track data dependencies only inside a single basic block */
			if (!(b->flags & ZEND_BB_FOLLOW) ||
			    (b->flags & ZEND_BB_TARGET)) {
				/* Skip continuation of "extended" BB */
				memset(Tsource, 0, (op_array->last_var + op_array->T) * sizeof(zend_op *));
			}
			zend_optimize_block(b, op_array, usage, &cfg, Tsource, &opt_count);
		}

		/* Eliminate NOPs */
		for (b = blocks; b < end; b++) {
			if (b->flags & (ZEND_BB_REACHABLE|ZEND_BB_UNREACHABLE_FREE)) {
				strip_nops(op_array, b);
			}
		}

		opt_count = 0;

		/* Jump optimization for each block */
		for (b = blocks; b < end; b++) {
			if (b->flags & ZEND_BB_REACHABLE) {
				zend_jmp_optimization(b, op_array, &cfg, jmp_hitlist, &opt_count);
			}
		}

		/* Eliminate unreachable basic blocks */
		zend_cfg_remark_reachable_blocks(op_array, &cfg);

		/* Merge Blocks */
		zend_merge_blocks(op_array, &cfg, &opt_count);

		if (opt_count == 0) {
			break;
		}
	}

	assemble_code_blocks(&cfg, op_array, ctx);

	if (ctx->debug_level & ZEND_DUMP_AFTER_BLOCK_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG | ZEND_DUMP_HIDE_UNREACHABLE, "after block pass", &cfg);
	}

	/* Destroy CFG */
	zend_arena_release(&ctx->arena, checkpoint);
}
