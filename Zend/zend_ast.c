/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2014 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend_ast.h"
#include "zend_API.h"
#include "zend_operators.h"

ZEND_API zend_ast *zend_ast_create_constant(zval *zv)
{
	zend_ast_zval *ast = emalloc(sizeof(zend_ast_zval));
	ast->kind = ZEND_CONST;
	ast->attr = 0;
	ZVAL_COPY_VALUE(&ast->val, zv);
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_znode(znode *node)
{
	zend_ast_znode *ast = emalloc(sizeof(zend_ast_znode));
	ast->kind = ZEND_AST_ZNODE;
	ast->attr = 0;
	ast->node = *node;
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_zval_ex(zval *zv, zend_ast_attr attr)
{
	zend_ast_zval *ast = emalloc(sizeof(zend_ast_zval));
	ast->kind = ZEND_CONST;
	ast->attr = attr;
	ZVAL_COPY_VALUE(&ast->val, zv);
	return (zend_ast *) ast;
}

ZEND_API zend_ast* zend_ast_create_unary_ex(zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0)
{
	zend_ast *ast = emalloc(sizeof(zend_ast));
	ast->kind = kind;
	ast->attr = attr;
	ast->children = 1;
	ast->child[0] = op0;
	return ast;
}

ZEND_API zend_ast* zend_ast_create_binary_ex(
	zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0, zend_ast *op1
) {
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast *));
	ast->kind = kind;
	ast->attr = attr;
	ast->children = 2;
	ast->child[0] = op0;
	ast->child[1] = op1;
	return ast;
}

ZEND_API zend_ast* zend_ast_create_ternary_ex(
	zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0, zend_ast *op1, zend_ast *op2
) {
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast *) * 2);
	ast->kind = kind;
	ast->attr = attr;
	ast->children = 3;
	ast->child[0] = op0;
	ast->child[1] = op1;
	ast->child[2] = op2;
	return ast;
}

ZEND_API zend_ast *zend_ast_create_dynamic(zend_ast_kind kind)
{
	/* use 4 children as default */
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast *) * 3);
	ast->kind = kind;
	ast->attr = 0;
	ast->children = 0;
	return ast;
}

static inline zend_bool is_power_of_two(unsigned short n) {
	return n == (n & -n);
}

ZEND_API zend_ast *zend_ast_dynamic_add(zend_ast *ast, zend_ast *op)
{
	if (ast->children >= 4 && is_power_of_two(ast->children)) {
		ast = erealloc(ast, sizeof(zend_ast) + sizeof(zend_ast *) * (ast->children * 2 - 1));
	}
	ast->child[ast->children++] = op;
	return ast;
}

ZEND_API void zend_ast_dynamic_shrink(zend_ast **ast)
{
	*ast = erealloc(*ast, sizeof(zend_ast) + sizeof(zend_ast *) * ((*ast)->children - 1));
}

ZEND_API int zend_ast_is_ct_constant(zend_ast *ast)
{
	int i;

	if (ast->kind == ZEND_CONST) {
		return !Z_CONSTANT_P(zend_ast_get_zval(ast));
	} else {
		for (i = 0; i < ast->children; i++) {
			if (ast->child[i]) {
				if (!zend_ast_is_ct_constant(ast->child[i])) {
					return 0;
				}
			}
		}
		return 1;
	}
}

static void zend_ast_add_array_element(zval *result, zval *offset, zval *expr TSRMLS_DC)
{
	switch (Z_TYPE_P(offset)) {
		case IS_UNDEF:
			zend_hash_next_index_insert(Z_ARRVAL_P(result), expr);
			break;
		case IS_STRING:
			zend_symtable_update(Z_ARRVAL_P(result), Z_STR_P(offset), expr);
//???
			zval_dtor(offset);
			break;
		case IS_NULL:
			zend_symtable_update(Z_ARRVAL_P(result), STR_EMPTY_ALLOC(), expr);
			break;
		case IS_LONG:
			zend_hash_index_update(Z_ARRVAL_P(result), Z_LVAL_P(offset), expr);
			break;
		case IS_FALSE:
			zend_hash_index_update(Z_ARRVAL_P(result), 0, expr);
			break;
		case IS_TRUE:
			zend_hash_index_update(Z_ARRVAL_P(result), 1, expr);
			break;
		case IS_DOUBLE:
			zend_hash_index_update(Z_ARRVAL_P(result), zend_dval_to_lval(Z_DVAL_P(offset)), expr);
			break;
		default:
			zend_error(E_ERROR, "Illegal offset type");
			break;
 	}
}

ZEND_API void zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope TSRMLS_DC)
{
	zval op1, op2;

	switch (ast->kind) {
		case ZEND_AST_BINARY_OP:
		{
			binary_op_type op = get_binary_op(ast->attr);
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, ast->child[1], scope TSRMLS_CC);
			op(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		}
		case ZEND_BW_NOT:
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			bitwise_not_function(result, &op1 TSRMLS_CC);
			zval_dtor(&op1);
			break;
		case ZEND_BOOL_NOT:
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			boolean_not_function(result, &op1 TSRMLS_CC);
			zval_dtor(&op1);
			break;
		case ZEND_CONST:
			ZVAL_DUP(result, zend_ast_get_zval(ast));
			if (Z_OPT_CONSTANT_P(result)) {
				zval_update_constant_ex(result, 1, scope TSRMLS_CC);
			}
			break;
		case ZEND_AST_AND:
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			if (zend_is_true(&op1 TSRMLS_CC)) {
				zend_ast_evaluate(&op2, ast->child[1], scope TSRMLS_CC);
				ZVAL_BOOL(result, zend_is_true(&op2 TSRMLS_CC));
				zval_dtor(&op2);
			} else {
				ZVAL_BOOL(result, 0);
			}
			zval_dtor(&op1);
			break;
		case ZEND_AST_OR:
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			if (zend_is_true(&op1 TSRMLS_CC)) {
				ZVAL_BOOL(result, 1);
			} else {
				zend_ast_evaluate(&op2, ast->child[1], scope TSRMLS_CC);
				ZVAL_BOOL(result, zend_is_true(&op2 TSRMLS_CC));
				zval_dtor(&op2);
			}
			zval_dtor(&op1);
			break;
		case ZEND_AST_CONDITIONAL:
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			if (zend_is_true(&op1 TSRMLS_CC)) {
				if (!ast->child[1]) {
					*result = op1;
				} else {
					zend_ast_evaluate(result, ast->child[1], scope TSRMLS_CC);
					zval_dtor(&op1);
				}
			} else {
				zend_ast_evaluate(result, ast->child[2], scope TSRMLS_CC);
				zval_dtor(&op1);
			}
			break;
		case ZEND_AST_UNARY_PLUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, ast->child[0], scope TSRMLS_CC);
			add_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op2);
			break;
		case ZEND_AST_UNARY_MINUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, ast->child[0], scope TSRMLS_CC);
			sub_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op2);
			break;
		case ZEND_AST_ARRAY:
			array_init(result);
			{
				zend_uint i;
				for (i = 0; i < ast->children; i++) {
					zend_ast *elem = ast->child[i];
					if (elem->child[1]) {
						zend_ast_evaluate(&op1, elem->child[1], scope TSRMLS_CC);
					} else {
						ZVAL_UNDEF(&op1);
					}
					zend_ast_evaluate(&op2, elem->child[0], scope TSRMLS_CC);
					zend_ast_add_array_element(result, &op1, &op2 TSRMLS_CC);
				}
			}
			break;
		default:
			zend_error(E_ERROR, "Unsupported constant expression");
	}
}

ZEND_API zend_ast *zend_ast_copy(zend_ast *ast)
{
	if (ast == NULL) {
		return NULL;
	} else if (ast->kind == ZEND_CONST) {
		zend_ast *copy = zend_ast_create_constant(zend_ast_get_zval(ast));
		zval_copy_ctor(zend_ast_get_zval(copy));
		return copy;
	} else if (ast->children) {
		zend_ast *new = emalloc(sizeof(zend_ast) + sizeof(zend_ast *) * (ast->children - 1));
		int i;
		new->kind = ast->kind;
		new->children = ast->children;
		for (i = 0; i < ast->children; i++) {
			new->child[i] = zend_ast_copy(ast->child[i]);
		}
		return new;
	}
	return zend_ast_create_dynamic(ast->kind);
}

ZEND_API void zend_ast_destroy(zend_ast *ast)
{
	int i;

	if (ast->kind == ZEND_CONST) {
		zval_ptr_dtor(zend_ast_get_zval(ast));
	} else if (ast->kind != ZEND_AST_ZNODE) {
		for (i = 0; i < ast->children; i++) {
			if (ast->child[i]) {
				zend_ast_destroy(ast->child[i]);
			}
		}
	}
	efree(ast);
}
