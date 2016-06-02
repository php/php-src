/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
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
	zend_constant *c;
	char *lookup_name;
	int retval = 1;
	ALLOCA_FLAG(use_heap);

	if ((c = zend_hash_find_ptr(EG(zend_constants), name)) == NULL) {
		lookup_name = do_alloca(ZSTR_LEN(name) + 1, use_heap);
		memcpy(lookup_name, ZSTR_VAL(name), ZSTR_LEN(name) + 1);
		zend_str_tolower(lookup_name, ZSTR_LEN(name));

		if ((c = zend_hash_str_find_ptr(EG(zend_constants), lookup_name, ZSTR_LEN(name))) != NULL) {
			if (!(c->flags & CONST_CT_SUBST) || (c->flags & CONST_CS)) {
				retval = 0;
			}
		} else {
			retval = 0;
		}
		free_alloca(lookup_name, use_heap);
	}

	if (retval) {
		if (c->flags & CONST_PERSISTENT) {
			ZVAL_COPY_VALUE(result, &c->value);
			if (copy) {
				zval_copy_ctor(result);
			}
		} else {
			retval = 0;
		}
	}

	return retval;
}

/* CFG back references management */

#define DEL_SOURCE(from, to)
#define ADD_SOURCE(from, to)

/* Data dependencies macros */

#define VAR_NUM_EX(op) VAR_NUM((op).var)

#define VAR_SOURCE(op) Tsource[VAR_NUM(op.var)]
#define SET_VAR_SOURCE(opline) Tsource[VAR_NUM(opline->result.var)] = opline

#define convert_to_string_safe(v) \
	if (Z_TYPE_P((v)) == IS_NULL) { \
		ZVAL_STRINGL((v), "", 0); \
	} else { \
		convert_to_string((v)); \
	}

static void strip_leading_nops(zend_op_array *op_array, zend_basic_block *b)
{
	zend_op *opcodes = op_array->opcodes;

	while (b->len > 0 && opcodes[b->start].opcode == ZEND_NOP) {
		b->start++;
		b->len--;
	}
}

static void strip_nops(zend_op_array *op_array, zend_basic_block *b)
{
	uint32_t i, j;

	strip_leading_nops(op_array, b);
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

static void zend_optimize_block(zend_basic_block *block, zend_op_array *op_array, zend_bitset used_ext, zend_cfg *cfg, zend_op **Tsource)
{
	zend_op *opline, *src;
	zend_op *end, *last_op = NULL;

	/* remove leading NOPs */
	strip_leading_nops(op_array, block);

	opline = op_array->opcodes + block->start;
	end = opline + block->len;
	while (opline < end) {
		/* Constant Propagation: strip X = QM_ASSIGN(const) */
		if ((opline->op1_type & (IS_TMP_VAR|IS_VAR)) &&
		    opline->opcode != ZEND_FREE) {
			src = VAR_SOURCE(opline->op1);
			if (src &&
			    src->opcode == ZEND_QM_ASSIGN &&
			    src->op1_type == IS_CONST) {

				znode_op op1 = opline->op1;
				zval c = ZEND_OP1_LITERAL(src);

				zval_copy_ctor(&c);
				if (zend_optimizer_update_op1_const(op_array, opline, &c)) {
					zend_optimizer_remove_live_range(op_array, op1.var);
					VAR_SOURCE(op1) = NULL;
					literal_dtor(&ZEND_OP1_LITERAL(src));
					MAKE_NOP(src);
				}
			}
		}

		/* Constant Propagation: strip X = QM_ASSIGN(const) */
		if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
			src = VAR_SOURCE(opline->op2);
			if (src &&
			    src->opcode == ZEND_QM_ASSIGN &&
			    src->op1_type == IS_CONST) {

				znode_op op2 = opline->op2;
				zval c = ZEND_OP1_LITERAL(src);

				zval_copy_ctor(&c);
				if (zend_optimizer_update_op2_const(op_array, opline, &c)) {
					zend_optimizer_remove_live_range(op_array, op2.var);
					VAR_SOURCE(op2) = NULL;
					literal_dtor(&ZEND_OP1_LITERAL(src));
					MAKE_NOP(src);
				}
			}
		}

		if (opline->opcode == ZEND_ECHO) {
			if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
				src = VAR_SOURCE(opline->op1);
				if (src &&
				    src->opcode == ZEND_CAST &&
				    src->extended_value == IS_STRING) {
					/* T = CAST(X, String), ECHO(T) => NOP, ECHO(X) */
					VAR_SOURCE(opline->op1) = NULL;
					COPY_NODE(opline->op1, src->op1);
					MAKE_NOP(src);
				}
			}

			if (opline->op1_type == IS_CONST) {
				if (last_op && last_op->opcode == ZEND_ECHO &&
				    last_op->op1_type == IS_CONST &&
				    Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_DOUBLE &&
				    Z_TYPE(ZEND_OP1_LITERAL(last_op)) != IS_DOUBLE) {
					/* compress consecutive ECHO's.
					 * Float to string conversion may be affected by current
					 * locale setting.
					 */
					int l, old_len;

					if (Z_TYPE(ZEND_OP1_LITERAL(opline)) != IS_STRING) {
						convert_to_string_safe(&ZEND_OP1_LITERAL(opline));
					}
					if (Z_TYPE(ZEND_OP1_LITERAL(last_op)) != IS_STRING) {
						convert_to_string_safe(&ZEND_OP1_LITERAL(last_op));
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
					zval_dtor(&ZEND_OP1_LITERAL(opline));
					ZVAL_STR(&ZEND_OP1_LITERAL(opline), zend_new_interned_string(Z_STR(ZEND_OP1_LITERAL(last_op))));
					ZVAL_NULL(&ZEND_OP1_LITERAL(last_op));
					MAKE_NOP(last_op);
				}
				last_op = opline;
			} else {
				last_op = NULL;
			}
		} else {
			last_op = NULL;
		}

		switch (opline->opcode) {

			case ZEND_FREE:
				if (opline->op1_type == IS_TMP_VAR) {
					src = VAR_SOURCE(opline->op1);
					if (src &&
					    (src->opcode == ZEND_BOOL || src->opcode == ZEND_BOOL_NOT)) {
						/* T = BOOL(X), FREE(T) => NOP */
						if (ZEND_OP1_TYPE(src) == IS_CONST) {
							literal_dtor(&ZEND_OP1_LITERAL(src));
						}
						VAR_SOURCE(opline->op1) = NULL;
						MAKE_NOP(src);
						MAKE_NOP(opline);
					}
				} else if (opline->op1_type == IS_VAR) {
					src = VAR_SOURCE(opline->op1);
					/* V = OP, FREE(V) => OP. NOP */
					if (src &&
					    src->opcode != ZEND_FETCH_R &&
					    src->opcode != ZEND_FETCH_STATIC_PROP_R &&
					    src->opcode != ZEND_FETCH_DIM_R &&
					    src->opcode != ZEND_FETCH_OBJ_R &&
					    src->opcode != ZEND_NEW) {
						if (opline->extended_value & ZEND_FREE_ON_RETURN) {
							/* mark as removed (empty live range) */
							op_array->live_range[opline->op2.num].var = (uint32_t)-1;
						}
						ZEND_RESULT_TYPE(src) = IS_UNUSED;
						MAKE_NOP(opline);
					}
				}
				break;

#if 0
		/* pre-evaluate functions:
		   constant(x)
		   defined(x)
		   function_exists(x)
		   extension_loaded(x)
		   BAD: interacts badly with Accelerator
		*/
		if((ZEND_OP1_TYPE(opline) & IS_VAR) &&
		   VAR_SOURCE(opline->op1) && VAR_SOURCE(opline->op1)->opcode == ZEND_DO_CF_FCALL &&
		   VAR_SOURCE(opline->op1)->extended_value == 1) {
			zend_op *fcall = VAR_SOURCE(opline->op1);
			zend_op *sv = fcall-1;
			if(sv >= block->start_opline && sv->opcode == ZEND_SEND_VAL &&
			   ZEND_OP1_TYPE(sv) == IS_CONST && Z_TYPE(OPLINE_OP1_LITERAL(sv)) == IS_STRING &&
			   Z_LVAL(OPLINE_OP2_LITERAL(sv)) == 1
			   ) {
				zval *arg = &OPLINE_OP1_LITERAL(sv);
				char *fname = FUNCTION_CACHE->funcs[Z_LVAL(ZEND_OP1_LITERAL(fcall))].function_name;
				int flen = FUNCTION_CACHE->funcs[Z_LVAL(ZEND_OP1_LITERAL(fcall))].name_len;
				if(flen == sizeof("defined")-1 && zend_binary_strcasecmp(fname, flen, "defined", sizeof("defined")-1) == 0) {
					zval c;
					if(zend_optimizer_get_persistent_constant(Z_STR_P(arg), &c, 0 ELS_CC) != 0) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						LITERAL_BOOL(opline->op1, 1);
						ZEND_OP1_TYPE(opline) = IS_CONST;
					}
				} else if((flen == sizeof("function_exists")-1 && zend_binary_strcasecmp(fname, flen, "function_exists", sizeof("function_exists")-1) == 0) ||
						  (flen == sizeof("is_callable")-1 && zend_binary_strcasecmp(fname, flen, "is_callable", sizeof("is_callable")-1) == 0)
						  ) {
					zend_function *function;
					if((function = zend_hash_find_ptr(EG(function_table), Z_STR_P(arg))) != NULL) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						LITERAL_BOOL(opline->op1, 1);
						ZEND_OP1_TYPE(opline) = IS_CONST;
					}
				} else if(flen == sizeof("constant")-1 && zend_binary_strcasecmp(fname, flen, "constant", sizeof("constant")-1) == 0) {
					zval c;
					if(zend_optimizer_get_persistent_constant(Z_STR_P(arg), &c, 1 ELS_CC) != 0) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						ZEND_OP1_LITERAL(opline) = zend_optimizer_add_literal(op_array, &c);
						/* no copy ctor - get already copied it */
						ZEND_OP1_TYPE(opline) = IS_CONST;
					}
				} else if(flen == sizeof("extension_loaded")-1 && zend_binary_strcasecmp(fname, flen, "extension_loaded", sizeof("extension_loaded")-1) == 0) {
					if(zend_hash_exists(&module_registry, Z_STR_P(arg))) {
						literal_dtor(arg);
						MAKE_NOP(sv);
						MAKE_NOP(fcall);
						LITERAL_BOOL(opline->op1, 1);
						ZEND_OP1_TYPE(opline) = IS_CONST;
					}
				}
			}
		}
#endif

			case ZEND_FETCH_LIST:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					/* LIST variable will be deleted later by FREE */
					Tsource[VAR_NUM(opline->op1.var)] = NULL;
				}
				break;

			case ZEND_CASE:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					/* CASE variable will be deleted later by FREE, so we can't optimize it */
					Tsource[VAR_NUM(opline->op1.var)] = NULL;
					break;
				}
				if (opline->op1_type == IS_CONST &&
				    opline->op2_type == IS_CONST) {
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
								goto optimize_bool;
							case ZEND_BOOL:
								/* T = BOOL(X) + BOOL(T) -> NOP, BOOL(X) */
								VAR_SOURCE(opline->op1) = NULL;
								COPY_NODE(opline->op1, src->op1);
								MAKE_NOP(src);
								goto optimize_bool;
							case ZEND_IS_EQUAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_NOT_EQUAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								break;
							case ZEND_IS_NOT_EQUAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_EQUAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								break;
							case ZEND_IS_IDENTICAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_NOT_IDENTICAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								break;
							case ZEND_IS_NOT_IDENTICAL:
								if (opline->opcode == ZEND_BOOL_NOT) {
									src->opcode = ZEND_IS_IDENTICAL;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
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
								break;
							case ZEND_ISSET_ISEMPTY_VAR:
							case ZEND_ISSET_ISEMPTY_DIM_OBJ:
							case ZEND_ISSET_ISEMPTY_PROP_OBJ:
							case ZEND_ISSET_ISEMPTY_STATIC_PROP:
							case ZEND_INSTANCEOF:
							case ZEND_TYPE_CHECK:
							case ZEND_DEFINED:
								if (opline->opcode == ZEND_BOOL_NOT) {
									break;
								}
								COPY_NODE(src->result, opline->result);
								SET_VAR_SOURCE(src);
								MAKE_NOP(opline);
								break;
						}
					}
				}
				break;

			case ZEND_JMPZ:
			case ZEND_JMPNZ:
			case ZEND_JMPZ_EX:
			case ZEND_JMPNZ_EX:
			case ZEND_JMPZNZ:
			optimize_jmpznz:
				if (opline->op1_type == IS_TMP_VAR &&
				    (!zend_bitset_in(used_ext, VAR_NUM(opline->op1.var)) ||
				     (opline->result_type == opline->op1_type &&
				      opline->result.var == opline->op1.var))) {
					src = VAR_SOURCE(opline->op1);
					if (src) {
						if (src->opcode == ZEND_BOOL_NOT &&
						    opline->opcode != ZEND_JMPZ_EX &&
						    opline->opcode != ZEND_JMPNZ_EX) {
							VAR_SOURCE(opline->op1) = NULL;
							COPY_NODE(opline->op1, src->op1);
							if (opline->opcode == ZEND_JMPZ) {
								/* T = BOOL_NOT(X) + JMPZ(T) -> NOP, JMPNZ(X) */
								opline->opcode = ZEND_JMPNZ;
							} else if (opline->opcode == ZEND_JMPNZ) {
								/* T = BOOL_NOT(X) + JMPNZ(T) -> NOP, JMPZ(X) */
								opline->opcode = ZEND_JMPZ;
#if 0
							} else if (opline->opcode == ZEND_JMPZ_EX) {
								/* T = BOOL_NOT(X) + JMPZ_EX(T) -> NOP, JMPNZ_EX(X) */
								opline->opcode = ZEND_JMPNZ_EX;
							} else if (opline->opcode == ZEND_JMPNZ_EX) {
								/* T = BOOL_NOT(X) + JMPNZ_EX(T) -> NOP, JMPZ_EX(X) */
								opline->opcode = ZEND_JMPZ;
#endif
							} else {
								/* T = BOOL_NOT(X) + JMPZNZ(T,L1,L2) -> NOP, JMPZNZ(X,L2,L1) */
								uint32_t tmp;

								ZEND_ASSERT(opline->opcode == ZEND_JMPZNZ);
								tmp = block->successors[0];
								block->successors[0] = block->successors[1];
								block->successors[1] = tmp;
							}
							MAKE_NOP(src);
							goto optimize_jmpznz;
						} else if (src->opcode == ZEND_BOOL ||
						           src->opcode == ZEND_QM_ASSIGN) {
							VAR_SOURCE(opline->op1) = NULL;
							COPY_NODE(opline->op1, src->op1);
							MAKE_NOP(src);
							goto optimize_jmpznz;
						}
					}
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
							convert_to_string_safe(&ZEND_OP2_LITERAL(opline));
						}
						if (Z_TYPE(ZEND_OP2_LITERAL(src)) != IS_STRING) {
							convert_to_string_safe(&ZEND_OP2_LITERAL(src));
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
						zend_string_release(Z_STR(ZEND_OP2_LITERAL(opline)));
						ZVAL_STR(&ZEND_OP2_LITERAL(opline), zend_new_interned_string(Z_STR(ZEND_OP2_LITERAL(src))));
						ZVAL_NULL(&ZEND_OP2_LITERAL(src));
						MAKE_NOP(src);
					}
				}

				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op1);
					if (src &&
					    src->opcode == ZEND_CAST &&
					    src->extended_value == IS_STRING) {
						/* convert T1 = CAST(STRING, X), T2 = CONCAT(T1, Y) to T2 = CONCAT(X,Y) */
						VAR_SOURCE(opline->op1) = NULL;
						COPY_NODE(opline->op1, src->op1);
						MAKE_NOP(src);
					}
	            }
				if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op2);
					if (src &&
					    src->opcode == ZEND_CAST &&
					    src->extended_value == IS_STRING) {
						/* convert T1 = CAST(STRING, X), T2 = CONCAT(Y, T1) to T2 = CONCAT(Y,X) */
						zend_op *src = VAR_SOURCE(opline->op2);
						VAR_SOURCE(opline->op2) = NULL;
						COPY_NODE(opline->op2, src->op1);
						MAKE_NOP(src);
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
				} else if (opline->op2_type == IS_CONST &&
			           Z_TYPE(ZEND_OP2_LITERAL(opline)) == IS_STRING &&
			           Z_STRLEN(ZEND_OP2_LITERAL(opline)) == 0) {
					/* convert CONCAT(X, '') => CAST(STRING, X) */
					literal_dtor(&ZEND_OP2_LITERAL(opline));
					opline->opcode = ZEND_CAST;
					opline->extended_value = IS_STRING;
					opline->op2_type = IS_UNUSED;
					opline->op2.var = 0;
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
					binary_op_type binary_op;
					zval result;
					int er;

optimize_constant_binary_op:
					binary_op = get_binary_op(opline->opcode);
		            if ((opline->opcode == ZEND_DIV || opline->opcode == ZEND_MOD) &&
		                zval_get_long(&ZEND_OP2_LITERAL(opline)) == 0) {
						SET_VAR_SOURCE(opline);
		                opline++;
						continue;
		            } else if ((opline->opcode == ZEND_SL || opline->opcode == ZEND_SR) &&
		                zval_get_long(&ZEND_OP2_LITERAL(opline)) < 0) {
						SET_VAR_SOURCE(opline);
		                opline++;
						continue;
					} else if (zend_binary_op_produces_numeric_string_error(opline->opcode, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline))) {
						SET_VAR_SOURCE(opline);
		                opline++;
						continue;
					}
					er = EG(error_reporting);
					EG(error_reporting) = 0;
					if (binary_op(&result, &ZEND_OP1_LITERAL(opline), &ZEND_OP2_LITERAL(opline)) == SUCCESS) {
						literal_dtor(&ZEND_OP1_LITERAL(opline));
						literal_dtor(&ZEND_OP2_LITERAL(opline));
						opline->opcode = ZEND_QM_ASSIGN;
						SET_UNUSED(opline->op2);
						zend_optimizer_update_op1_const(op_array, opline, &result);
					}
					EG(error_reporting) = er;
				}
				break;

			case ZEND_BW_NOT:
				if (opline->op1_type == IS_CONST) {
					/* evaluate constant unary ops */
					unary_op_type unary_op;
					zval result;

optimize_const_unary_op:
					unary_op = get_unary_op(opline->opcode);
					if (unary_op) {
						unary_op(&result, &ZEND_OP1_LITERAL(opline));
						literal_dtor(&ZEND_OP1_LITERAL(opline));
					} else {
						/* BOOL */
						result = ZEND_OP1_LITERAL(opline);
						convert_to_boolean(&result);
						ZVAL_NULL(&ZEND_OP1_LITERAL(opline));
					}
					opline->opcode = ZEND_QM_ASSIGN;
					zend_optimizer_update_op1_const(op_array, opline, &result);
				}
				break;

			case ZEND_RETURN:
			case ZEND_EXIT:
				if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
					src = VAR_SOURCE(opline->op1);
					if (src && src->opcode == ZEND_QM_ASSIGN) {
						/* T = QM_ASSIGN(X), RETURN(T) to NOP, RETURN(X) */
						VAR_SOURCE(opline->op1) = NULL;
						COPY_NODE(opline->op1, src->op1);
						MAKE_NOP(src);
					}
				}
				break;

			case ZEND_QM_ASSIGN:
				if (opline->op1_type == opline->result_type &&
				    opline->op1.var == opline->result.var) {
					/* strip T = QM_ASSIGN(T) */
					MAKE_NOP(opline);
				}
				break;
		}

		/* get variable source */
		if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
			SET_VAR_SOURCE(opline);
		}
		opline++;
	}

	strip_nops(op_array, block);
}

/* Rebuild plain (optimized) op_array from CFG */
static void assemble_code_blocks(zend_cfg *cfg, zend_op_array *op_array)
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
		if (b->flags & ZEND_BB_REACHABLE) {
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
				if (ZEND_OP1_TYPE(op) == IS_CONST) {
					literal_dtor(&ZEND_OP1_LITERAL(op));
				}
				if (ZEND_OP2_TYPE(op) == IS_CONST) {
					literal_dtor(&ZEND_OP2_LITERAL(op));
				}
			}
		}
	}

	new_opcodes = emalloc(len * sizeof(zend_op));
	opline = new_opcodes;

	/* Copy code of reachable blocks into a single buffer */
	for (b = blocks; b < end; b++) {
		if (b->flags & ZEND_BB_REACHABLE) {
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
				ZEND_SET_OP_JMP_ADDR(opline, opline->op2, new_opcodes + blocks[b->successors[0]].start);
				break;
			case ZEND_CATCH:
				if (!opline->result.var) {
					opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[0]].start);
				}
				break;
			case ZEND_DECLARE_ANON_CLASS:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
			case ZEND_FE_FETCH_R:
			case ZEND_FE_FETCH_RW:
				opline->extended_value = ZEND_OPLINE_TO_OFFSET(opline, new_opcodes + blocks[b->successors[0]].start);
				break;
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

	/* adjust loop jump targets & remove unused live range entries */
	if (op_array->last_live_range) {
		int i, j;
		uint32_t *map;
		ALLOCA_FLAG(use_heap);

		map = (uint32_t *)do_alloca(sizeof(uint32_t) * op_array->last_live_range, use_heap);

		for (i = 0, j = 0; i < op_array->last_live_range; i++) {
			if (op_array->live_range[i].var == (uint32_t)-1) {
				/* this live range already removed */
				continue;
			}
			if (!(blocks[cfg->map[op_array->live_range[i].start]].flags & ZEND_BB_REACHABLE)) {
				ZEND_ASSERT(!(blocks[cfg->map[op_array->live_range[i].end]].flags & ZEND_BB_REACHABLE));
			} else {
				uint32_t start_op = blocks[cfg->map[op_array->live_range[i].start]].start;
				uint32_t end_op = blocks[cfg->map[op_array->live_range[i].end]].start;

				if (start_op == end_op) {
					/* skip empty live range */
					continue;
				}
				op_array->live_range[i].start = start_op;
				op_array->live_range[i].end = end_op;
				map[i] = j;
				if (i != j) {
					op_array->live_range[j]  = op_array->live_range[i];
				}
				j++;
			}
		}

		if (i != j) {
			if ((op_array->last_live_range = j)) {
				zend_op *opline = new_opcodes;
				zend_op *end = opline + len;
				while (opline != end) {
					if ((opline->opcode == ZEND_FREE || opline->opcode == ZEND_FE_FREE) &&
							opline->extended_value == ZEND_FREE_ON_RETURN) {
						ZEND_ASSERT(opline->op2.num < (uint32_t) i);
						opline->op2.num = map[opline->op2.num];
					}
					opline++;
				}
			} else {
				efree(op_array->live_range);
				op_array->live_range = NULL;
			}
		}
		free_alloca(map, use_heap);
	}

	/* adjust early binding list */
	if (op_array->early_binding != (uint32_t)-1) {
		uint32_t *opline_num = &op_array->early_binding;
		zend_op *end;

		opline = op_array->opcodes;
		end = opline + op_array->last;
		while (opline < end) {
			if (opline->opcode == ZEND_DECLARE_INHERITED_CLASS_DELAYED) {
				*opline_num = opline - op_array->opcodes;
				opline_num = &ZEND_RESULT(opline).opline_num;
			}
			++opline;
		}
		*opline_num = -1;
	}

	/* rebuild map (just for printing) */
	memset(cfg->map, -1, sizeof(int) * op_array->last);
	for (n = 0; n < cfg->blocks_count; n++) {
		if (cfg->blocks[n].flags & ZEND_BB_REACHABLE) {
			cfg->map[cfg->blocks[n].start] = n;
		}
	}
}

static void zend_jmp_optimization(zend_basic_block *block, zend_op_array *op_array, zend_cfg *cfg, zend_uchar *same_t)
{
	/* last_op is the last opcode of the current block */
	zend_basic_block *blocks = cfg->blocks;
	zend_op *last_op;

	if (block->len == 0) {
		return;
	}

	last_op = op_array->opcodes + block->start + block->len - 1;
	switch (last_op->opcode) {
		case ZEND_JMP:
			{
				zend_basic_block *target_block = blocks + block->successors[0];
				zend_op *target = op_array->opcodes + target_block->start;
				int next = (block - blocks) + 1;

				while (next < cfg->blocks_count && !(blocks[next].flags & ZEND_BB_REACHABLE)) {
					/* find used one */
					next++;
				}

				/* JMP(next) -> NOP */
				if (block->successors[0] == next) {
					MAKE_NOP(last_op);
					block->len--;
					break;
				}

				if (target->opcode == ZEND_JMP &&
					block->successors[0] != target_block->successors[0] &&
					!(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMP L, L: JMP L1 -> JMP L1 */
					*last_op = *target;
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMPZNZ &&
				           !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMP L, L: JMPZNZ L1,L2 -> JMPZNZ L1,L2 */
					*last_op = *target;
					if (ZEND_OP1_TYPE(last_op) == IS_CONST) {
						zval zv = ZEND_OP1_LITERAL(last_op);
						zval_copy_ctor(&zv);
						last_op->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					block->successors[1] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
					ADD_SOURCE(block, block->successors[1]);
				} else if ((target->opcode == ZEND_RETURN ||
				            target->opcode == ZEND_RETURN_BY_REF ||
				            target->opcode == ZEND_EXIT) &&
				           !(op_array->fn_flags & ZEND_ACC_HAS_FINALLY_BLOCK)) {
					/* JMP L, L: RETURN to immediate RETURN */
					*last_op = *target;
					if (ZEND_OP1_TYPE(last_op) == IS_CONST) {
						zval zv = ZEND_OP1_LITERAL(last_op);
						zval_copy_ctor(&zv);
						last_op->op1.constant = zend_optimizer_add_literal(op_array, &zv);
					}
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = -1;
#if 0
				/* Temporarily disabled - see bug #0025274 */
				} else if (0&& block->op1_to != block &&
			           block->op1_to != blocks &&
						   op_array->last_try_catch == 0 &&
				           target->opcode != ZEND_FREE) {
				    /* Block Reordering (saves one JMP on each "for" loop iteration)
				     * It is disabled for some cases (ZEND_FREE)
				     * which may break register allocation.
            	     */
					zend_bool can_reorder = 0;
					zend_block_source *cs = block->op1_to->sources;

					/* the "target" block doesn't had any followed block */
					while(cs) {
						if (cs->from->follow_to == block->op1_to) {
							can_reorder = 0;
							break;
						}
						cs = cs->next;
					}
					if (can_reorder) {
						next = block->op1_to;
						/* the "target" block is not followed by current "block" */
						while (next->follow_to != NULL) {
							if (next->follow_to == block) {
								can_reorder = 0;
								break;
							}
							next = next->follow_to;
						}
						if (can_reorder) {
							zend_basic_block *prev = blocks;

							while (prev->next != block->op1_to) {
								prev = prev->next;
							}
							prev->next = next->next;
							next->next = block->next;
							block->next = block->op1_to;

							block->follow_to = block->op1_to;
							block->op1_to = NULL;
							MAKE_NOP(last_op);
							block->len--;
							if(block->len == 0) {
								/* this block is nothing but NOP now */
								delete_code_block(block, ctx);
							}
							break;
						}
					}
#endif
				}
			}
			break;

		case ZEND_JMPZ:
		case ZEND_JMPNZ:
			/* constant conditional JMPs */
			if (ZEND_OP1_TYPE(last_op) == IS_CONST) {
				int should_jmp = zend_is_true(&ZEND_OP1_LITERAL(last_op));

				if (last_op->opcode == ZEND_JMPZ) {
					should_jmp = !should_jmp;
				}
				literal_dtor(&ZEND_OP1_LITERAL(last_op));
				ZEND_OP1_TYPE(last_op) = IS_UNUSED;
				if (should_jmp) {
					/* JMPNZ(true) -> JMP */
					last_op->opcode = ZEND_JMP;
					DEL_SOURCE(block, block->successors[1]);
					block->successors[1] = -1;
				} else {
					/* JMPNZ(false) -> NOP */
					MAKE_NOP(last_op);
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = block->successors[1];
					block->successors[1] = -1;
				}
				break;
			}

			if (block->successors[0] == block->successors[1]) {
				/* L: JMP[N]Z(X, L+1) -> NOP or FREE(X) */

				if (last_op->op1_type & (IS_VAR|IS_TMP_VAR)) {
					last_op->opcode = ZEND_FREE;
					last_op->op2.num = 0;
				} else {
					MAKE_NOP(last_op);
				}
				block->successors[1] = -1;
				break;
			}

			if (1) {
				zend_uchar same_type = ZEND_OP1_TYPE(last_op);
				uint32_t same_var = VAR_NUM_EX(last_op->op1);
				zend_op *target;
				zend_op *target_end;
				zend_basic_block *target_block = blocks + block->successors[0];

next_target:
				target = op_array->opcodes + target_block->start;
				target_end = target + target_block->len;
				while (target < target_end && target->opcode == ZEND_NOP) {
					target++;
				}

				/* next block is only NOP's */
				if (target == target_end) {
					target_block = blocks + target_block->successors[0];
					goto next_target;
				} else if (target->opcode == INV_COND(last_op->opcode) &&
					/* JMPZ(X, L), L: JMPNZ(X, L2) -> JMPZ(X, L+1) */
				   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
				   same_type == ZEND_OP1_TYPE(target) &&
				   same_var == VAR_NUM_EX(target->op1) &&
				   !(target_block->flags & ZEND_BB_PROTECTED)
				   ) {
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == INV_COND_EX(last_op->opcode) &&
							(ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
				    		same_type == ZEND_OP1_TYPE(target) &&
				    		same_var == VAR_NUM_EX(target->op1) &&
							!(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMPZ(X, L), L: T = JMPNZ_EX(X, L2) -> T = JMPZ_EX(X, L+1) */
					last_op->opcode += 3;
					COPY_NODE(last_op->result, target->result);
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == last_op->opcode &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   same_type == ZEND_OP1_TYPE(target) &&
						   same_var == VAR_NUM_EX(target->op1) &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMPZ(X, L), L: JMPZ(X, L2) -> JMPZ(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMP &&
				           !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMPZ(X, L), L: JMP(L2) -> JMPZ(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMPZNZ &&
				           (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
				           same_type == ZEND_OP1_TYPE(target) &&
				           same_var == VAR_NUM_EX(target->op1) &&
				           !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* JMPZ(X, L), L: JMPZNZ(X, L2, L3) -> JMPZ(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					if (last_op->opcode == ZEND_JMPZ) {
						block->successors[0] = target_block->successors[0];
					} else {
						block->successors[0] = target_block->successors[1];
					}
					ADD_SOURCE(block, block->successors[0]);
				}
			}

			if (last_op->opcode == ZEND_JMPZ || last_op->opcode == ZEND_JMPNZ) {
				zend_op *target;
				zend_op *target_end;
				zend_basic_block *target_block;

				while (1) {
					target_block = blocks + block->successors[1];
					target = op_array->opcodes + target_block->start;
					target_end = op_array->opcodes + target_block->start + 1;
					while (target < target_end && target->opcode == ZEND_NOP) {
						target++;
					}

					/* next block is only NOP's */
					if (target == target_end && !(target_block->flags & ZEND_BB_PROTECTED)) {
						DEL_SOURCE(block, block->successors[1]);
						block->successors[1] = target_block->successors[0];
						ADD_SOURCE(block, block->successors[1]);
					} else {
						break;
					}
				}
				/* JMPZ(X,L1), JMP(L2) -> JMPZNZ(X,L1,L2) */
				if (target->opcode == ZEND_JMP &&
					!(target_block->flags & ZEND_BB_PROTECTED)) {
					DEL_SOURCE(block, block->successors[1]);
					if (last_op->opcode == ZEND_JMPZ) {
						block->successors[1] = target_block->successors[0];
						ADD_SOURCE(block, block->successors[1]);
					} else {
						block->successors[1] = block->successors[0];
						block->successors[0] = target_block->successors[0];
						ADD_SOURCE(block, block->successors[0]);
					}
					last_op->opcode = ZEND_JMPZNZ;
				}
			}
			break;

		case ZEND_JMPNZ_EX:
		case ZEND_JMPZ_EX:
			/* constant conditional JMPs */
			if (ZEND_OP1_TYPE(last_op) == IS_CONST) {
				int should_jmp = zend_is_true(&ZEND_OP1_LITERAL(last_op));

				if (last_op->opcode == ZEND_JMPZ_EX) {
					should_jmp = !should_jmp;
				}
				if (!should_jmp) {
					/* T = JMPZ_EX(true,L)   -> T = QM_ASSIGN(true)
					 * T = JMPNZ_EX(false,L) -> T = QM_ASSIGN(false)
					 */
					last_op->opcode = ZEND_QM_ASSIGN;
					SET_UNUSED(last_op->op2);
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = block->successors[1];
					block->successors[1] = -1;
				}
				break;
			}

			if (1) {
				zend_op *target, *target_end;
				zend_basic_block *target_block;
				int var_num = op_array->last_var + op_array->T;

				if (var_num <= 0) {
   					return;
				}
				memset(same_t, 0, var_num);
				same_t[VAR_NUM_EX(last_op->op1)] |= ZEND_OP1_TYPE(last_op);
				same_t[VAR_NUM_EX(last_op->result)] |= ZEND_RESULT_TYPE(last_op);
				target_block = blocks + block->successors[0];
next_target_ex:
				target = op_array->opcodes + target_block->start;
				target_end = target + target_block->len;
				while (target < target_end && target->opcode == ZEND_NOP) {
					target++;
				}
 				/* next block is only NOP's */
				if (target == target_end) {
					target_block = blocks + target_block->successors[0];
					goto next_target_ex;
				} else if (target->opcode == last_op->opcode-3 &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   (same_t[VAR_NUM_EX(target->op1)] & ZEND_OP1_TYPE(target)) != 0 &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L1), L1: JMPZ({X|T}, L2) -> T = JMPZ_EX(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == INV_EX_COND(last_op->opcode) &&
					   	   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   (same_t[VAR_NUM_EX(target->op1)] & ZEND_OP1_TYPE(target)) != 0 &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L1), L1: JMPNZ({X|T1}, L2) -> T = JMPZ_EX(X, L1+1) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == INV_EX_COND_EX(last_op->opcode) &&
					       (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   (same_t[VAR_NUM_EX(target->op1)] & ZEND_OP1_TYPE(target)) != 0 &&
						   (same_t[VAR_NUM_EX(target->result)] & ZEND_RESULT_TYPE(target)) != 0 &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L1), L1: T = JMPNZ_EX(T, L2) -> T = JMPZ_EX(X, L1+1) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == last_op->opcode &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   (same_t[VAR_NUM_EX(target->op1)] & ZEND_OP1_TYPE(target)) != 0 &&
						   (same_t[VAR_NUM_EX(target->result)] & ZEND_RESULT_TYPE(target)) != 0 &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L1), L1: T = JMPZ({X|T}, L2) -> T = JMPZ_EX(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMP &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L), L: JMP(L2) -> T = JMPZ(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMPZNZ &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   (same_t[VAR_NUM_EX(target->op1)] & ZEND_OP1_TYPE(target)) != 0 &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
					/* T = JMPZ_EX(X, L), L: JMPZNZ({X|T}, L2, L3) -> T = JMPZ_EX(X, L2) */
					DEL_SOURCE(block, block->successors[0]);
					if (last_op->opcode == ZEND_JMPZ_EX) {
						block->successors[0] = target_block->successors[0];
					} else {
						block->successors[0] = target_block->successors[1];
					}
					ADD_SOURCE(block, block->successors[0]);
				}
			}
			break;

		case ZEND_JMPZNZ: {
			int next = (block - blocks) + 1;

			while (next < cfg->blocks_count && !(blocks[next].flags & ZEND_BB_REACHABLE)) {
				/* find first accessed one */
				next++;
			}

			if (ZEND_OP1_TYPE(last_op) == IS_CONST) {
				if (!zend_is_true(&ZEND_OP1_LITERAL(last_op))) {
					/* JMPZNZ(false,L1,L2) -> JMP(L1) */
					literal_dtor(&ZEND_OP1_LITERAL(last_op));
					last_op->opcode = ZEND_JMP;
					SET_UNUSED(last_op->op1);
					SET_UNUSED(last_op->op2);
					DEL_SOURCE(block, block->successors[1]);
					block->successors[1] = -1;
				} else {
					/* JMPZNZ(true,L1,L2) -> JMP(L2) */
					literal_dtor(&ZEND_OP1_LITERAL(last_op));
					last_op->opcode = ZEND_JMP;
					SET_UNUSED(last_op->op1);
					SET_UNUSED(last_op->op2);
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = block->successors[1];
					block->successors[1] = -1;
				}
			} else if (block->successors[0] == block->successors[1]) {
				/* both goto the same one - it's JMP */
				if (!(last_op->op1_type & (IS_VAR|IS_TMP_VAR))) {
					/* JMPZNZ(?,L,L) -> JMP(L) */
					last_op->opcode = ZEND_JMP;
					SET_UNUSED(last_op->op1);
					SET_UNUSED(last_op->op2);
					block->successors[1] = -1;
				}
			} else if (block->successors[0] == next) {
				/* jumping to next on Z - can follow to it and jump only on NZ */
				/* JMPZNZ(X,L1,L2) L1: -> JMPNZ(X,L2) */
				last_op->opcode = ZEND_JMPNZ;
				block->successors[0] = block->successors[1];
				block->successors[1] = next;
				/* no need to add source */
			} else if (block->successors[1] == next) {
				/* jumping to next on NZ - can follow to it and jump only on Z */
				/* JMPZNZ(X,L1,L2) L2: -> JMPZ(X,L1) */
				last_op->opcode = ZEND_JMPZ;
				/* no need to add source */
			}

			if (last_op->opcode == ZEND_JMPZNZ) {
				zend_uchar same_type = ZEND_OP1_TYPE(last_op);
				zend_uchar same_var = VAR_NUM_EX(last_op->op1);
				zend_op *target;
				zend_op *target_end;
				zend_basic_block *target_block = blocks + block->successors[0];

next_target_znz:
				target = op_array->opcodes + target_block->start;
				target_end = target + target_block->len;
				while (target < target_end && target->opcode == ZEND_NOP) {
					target++;
				}
				/* next block is only NOP's */
				if (target == target_end) {
					target_block = blocks + target_block->successors[0];
					goto next_target_znz;
				} else if ((target->opcode == ZEND_JMPZ || target->opcode == ZEND_JMPZNZ) &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   same_type == ZEND_OP1_TYPE(target) &&
						   same_var == VAR_NUM_EX(target->op1) &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
				    /* JMPZNZ(X, L1, L2), L1: JMPZ(X, L3) -> JMPZNZ(X, L3, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMPNZ &&
						   (ZEND_OP1_TYPE(target) & (IS_TMP_VAR|IS_CV)) &&
						   same_type == ZEND_OP1_TYPE(target) &&
						   same_var == VAR_NUM_EX(target->op1) &&
						   !(target_block->flags & ZEND_BB_PROTECTED)) {
                    /* JMPZNZ(X, L1, L2), L1: X = JMPNZ(X, L3) -> JMPZNZ(X, L1+1, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[1];
					ADD_SOURCE(block, block->successors[0]);
				} else if (target->opcode == ZEND_JMP &&
					       !(target_block->flags & ZEND_BB_PROTECTED)) {
                    /* JMPZNZ(X, L1, L2), L1: JMP(L3) -> JMPZNZ(X, L3, L2) */
					DEL_SOURCE(block, block->successors[0]);
					block->successors[0] = target_block->successors[0];
					ADD_SOURCE(block, block->successors[0]);
				}
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
		} else if (block->successors[1] != -1) {
			zend_bitset_union(usage, used_ext, bitset_len);
		}
		next_block = block;

		while (opline >= end) {
			/* usage checks */
			if (opline->result_type == IS_VAR) {
				if (!zend_bitset_in(usage, VAR_NUM(opline->result.var))) {
					switch (opline->opcode) {
						case ZEND_ASSIGN_ADD:
						case ZEND_ASSIGN_SUB:
						case ZEND_ASSIGN_MUL:
						case ZEND_ASSIGN_DIV:
						case ZEND_ASSIGN_POW:
						case ZEND_ASSIGN_MOD:
						case ZEND_ASSIGN_SL:
						case ZEND_ASSIGN_SR:
						case ZEND_ASSIGN_CONCAT:
						case ZEND_ASSIGN_BW_OR:
						case ZEND_ASSIGN_BW_AND:
						case ZEND_ASSIGN_BW_XOR:
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
					}
				} else {
					zend_bitset_excl(usage, VAR_NUM(opline->result.var));
				}
			} else if (opline->result_type == IS_TMP_VAR) {
				if (!zend_bitset_in(usage, VAR_NUM(opline->result.var))) {
					switch (opline->opcode) {
						case ZEND_POST_INC:
						case ZEND_POST_DEC:
							opline->opcode -= 2;
							opline->result_type = IS_UNUSED;
							break;
						case ZEND_QM_ASSIGN:
						case ZEND_BOOL:
						case ZEND_BOOL_NOT:
							if (ZEND_OP1_TYPE(opline) == IS_CONST) {
								literal_dtor(&ZEND_OP1_LITERAL(opline));
							} else if (ZEND_OP1_TYPE(opline) == IS_TMP_VAR) {
								opline->opcode = ZEND_FREE;
							} else {
								MAKE_NOP(opline);
							}
							break;
						case ZEND_JMPZ_EX:
						case ZEND_JMPNZ_EX:
							opline->opcode -= 3;
							SET_UNUSED(opline->result);
							break;
						case ZEND_ADD_ARRAY_ELEMENT:
						case ZEND_ROPE_ADD:
							zend_bitset_incl(usage, VAR_NUM(opline->result.var));
							break;
					}
				} else {
					switch (opline->opcode) {
						case ZEND_ADD_ARRAY_ELEMENT:
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

static void zend_merge_blocks(zend_op_array *op_array, zend_cfg *cfg)
{
	int i;
	zend_basic_block *b, *bb;
	zend_basic_block *prev = NULL;

	for (i = 0; i < cfg->blocks_count; i++) {
		b = cfg->blocks + i;
		if (b->flags & ZEND_BB_REACHABLE) {
			if ((b->flags & ZEND_BB_FOLLOW) &&
			    !(b->flags & (ZEND_BB_TARGET | ZEND_BB_PROTECTED)) &&
			    prev &&
			    prev->successors[0] == i && prev->successors[1] == -1)
			{
				zend_op *last_op = op_array->opcodes + prev->start + prev->len - 1;
				if (prev->len != 0 && last_op->opcode == ZEND_JMP) {
					MAKE_NOP(last_op);
				}

				for (bb = prev + 1; bb != b; bb++) {
					zend_op *op = op_array->opcodes + bb->start;
					zend_op *end = op + bb->len;
					while (op < end) {
						if (ZEND_OP1_TYPE(op) == IS_CONST) {
							literal_dtor(&ZEND_OP1_LITERAL(op));
						}
						if (ZEND_OP2_TYPE(op) == IS_CONST) {
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
				prev->successors[0] = b->successors[0];
				prev->successors[1] = b->successors[1];

				/* unlink & make block empty and unreachable */
				b->flags = 0;
				b->len = 0;
				b->successors[0] = -1;
				b->successors[1] = -1;
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
	zend_uchar *same_t;

    /* Build CFG */
	checkpoint = zend_arena_checkpoint(ctx->arena);
	if (zend_build_cfg(&ctx->arena, op_array, ZEND_CFG_SPLIT_AT_LIVE_RANGES, &cfg, NULL) != SUCCESS) {
		zend_arena_release(&ctx->arena, checkpoint);
		return;
	}

	if (ctx->debug_level & ZEND_DUMP_BEFORE_BLOCK_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG, "before block pass", &cfg);
	}

	if (op_array->last_var || op_array->T) {
		bitset_len = zend_bitset_len(op_array->last_var + op_array->T);
		Tsource = zend_arena_calloc(&ctx->arena, op_array->last_var + op_array->T, sizeof(zend_op *));
		same_t = zend_arena_alloc(&ctx->arena, op_array->last_var + op_array->T);
		usage = zend_arena_alloc(&ctx->arena, bitset_len * ZEND_BITSET_ELM_SIZE);
	} else {
		bitset_len = 0;
		Tsource = NULL;
		same_t = NULL;
		usage = NULL;
	}
	blocks = cfg.blocks;
	end = blocks + cfg.blocks_count;
	for (pass = 0; pass < PASSES; pass++) {
		/* Compute data dependencies */
		zend_bitset_clear(usage, bitset_len);
		zend_t_usage(&cfg, op_array, usage, ctx);

		/* optimize each basic block separately */
		for (b = blocks; b < end; b++) {
			if (!(b->flags & ZEND_BB_REACHABLE)) {
				continue;
			}
			/* we track data dependencies only insight a single basic block */
			if (!(b->flags & ZEND_BB_FOLLOW) ||
			    (b->flags & ZEND_BB_TARGET)) {
				/* Skip continuation of "extended" BB */
				memset(Tsource, 0, (op_array->last_var + op_array->T) * sizeof(zend_op *));
			}
			zend_optimize_block(b, op_array, usage, &cfg, Tsource);
		}

		/* Jump optimization for each block */
		for (b = blocks; b < end; b++) {
			if (b->flags & ZEND_BB_REACHABLE) {
				zend_jmp_optimization(b, op_array, &cfg, same_t);
			}
		}

		/* Eliminate unreachable basic blocks */
		zend_cfg_remark_reachable_blocks(op_array, &cfg);

		/* Merge Blocks */
		zend_merge_blocks(op_array, &cfg);
	}

	zend_bitset_clear(usage, bitset_len);
	zend_t_usage(&cfg, op_array, usage, ctx);
	assemble_code_blocks(&cfg, op_array);

	if (ctx->debug_level & ZEND_DUMP_AFTER_BLOCK_PASS) {
		zend_dump_op_array(op_array, ZEND_DUMP_CFG | ZEND_DUMP_HIDE_UNREACHABLE, "after block pass", &cfg);
	}

	/* Destroy CFG */
	zend_arena_release(&ctx->arena, checkpoint);
}
