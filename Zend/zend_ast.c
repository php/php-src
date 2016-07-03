/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
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
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zval));
	ast->kind = ZEND_CONST;
	ast->children = 0;
	ast->u.val = (zval*)(ast + 1);
	INIT_PZVAL_COPY(ast->u.val, zv);
	return ast;
}

ZEND_API zend_ast* zend_ast_create_unary(uint kind, zend_ast *op0)
{
	zend_ast *ast = emalloc(sizeof(zend_ast));
	ast->kind = kind;
	ast->children = 1;
	(&ast->u.child)[0] = op0;
	return ast;
}

ZEND_API zend_ast* zend_ast_create_binary(uint kind, zend_ast *op0, zend_ast *op1)
{
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast*));
	ast->kind = kind;
	ast->children = 2;
	(&ast->u.child)[0] = op0;
	(&ast->u.child)[1] = op1;
	return ast;
}

ZEND_API zend_ast* zend_ast_create_ternary(uint kind, zend_ast *op0, zend_ast *op1, zend_ast *op2)
{
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast*) * 2);
	ast->kind = kind;
	ast->children = 3;
	(&ast->u.child)[0] = op0;
	(&ast->u.child)[1] = op1;
	(&ast->u.child)[2] = op2;
	return ast;
}

ZEND_API zend_ast* zend_ast_create_dynamic(uint kind)
{
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast*) * 3); /* use 4 children as deafult */
	ast->kind = kind;
	ast->children = 0;
	return ast;
}

ZEND_API void zend_ast_dynamic_add(zend_ast **ast, zend_ast *op)
{
	if ((*ast)->children >= 4 && (*ast)->children == ((*ast)->children & -(*ast)->children)) {
		*ast = erealloc(*ast, sizeof(zend_ast) + sizeof(zend_ast*) * ((*ast)->children * 2 + 1));
	}
	(&(*ast)->u.child)[(*ast)->children++] = op;
}

ZEND_API void zend_ast_dynamic_shrink(zend_ast **ast)
{
	*ast = erealloc(*ast, sizeof(zend_ast) + sizeof(zend_ast*) * ((*ast)->children - 1));
}

ZEND_API int zend_ast_is_ct_constant(zend_ast *ast)
{
	int i;

	if (ast->kind == ZEND_CONST) {
		return !IS_CONSTANT_TYPE(Z_TYPE_P(ast->u.val));
	} else {
		for (i = 0; i < ast->children; i++) {
			if ((&ast->u.child)[i]) {
				if (!zend_ast_is_ct_constant((&ast->u.child)[i])) {
					return 0;
				}
			}
		}
		return 1;
	}
}

ZEND_API void zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope TSRMLS_DC)
{
	zval op1, op2;

	switch (ast->kind) {
		case ZEND_ADD:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			add_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_SUB:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			sub_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_MUL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			mul_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_POW:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			pow_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_DIV:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			div_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_MOD:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			mod_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_SL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			shift_left_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_SR:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			shift_right_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_CONCAT:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			concat_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_BW_OR:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			bitwise_or_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_BW_AND:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			bitwise_and_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_BW_XOR:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			bitwise_xor_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_BW_NOT:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			bitwise_not_function(result, &op1 TSRMLS_CC);
			zval_dtor(&op1);
			break;
		case ZEND_BOOL_NOT:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			boolean_not_function(result, &op1 TSRMLS_CC);
			zval_dtor(&op1);
			break;
		case ZEND_BOOL_XOR:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			boolean_xor_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_IDENTICAL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_identical_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_NOT_IDENTICAL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_not_identical_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_EQUAL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_equal_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_NOT_EQUAL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_not_equal_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_SMALLER:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_smaller_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_IS_SMALLER_OR_EQUAL:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			is_smaller_or_equal_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		case ZEND_CONST:
			/* class constants may be updated in-place */
			if (scope) {
				if (IS_CONSTANT_TYPE(Z_TYPE_P(ast->u.val))) {
					zval_update_constant_ex(&ast->u.val, 1, scope TSRMLS_CC);
				}
				*result = *ast->u.val;
				zval_copy_ctor(result);
			} else {
				*result = *ast->u.val;
				zval_copy_ctor(result);
				if (IS_CONSTANT_TYPE(Z_TYPE_P(result))) {
					zval_update_constant_ex(&result, 1, scope TSRMLS_CC);
				}
			}
			break;
		case ZEND_BOOL_AND:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			if (zend_is_true(&op1)) {
				zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
				ZVAL_BOOL(result, zend_is_true(&op2));
				zval_dtor(&op2);
			} else {
				ZVAL_BOOL(result, 0);
			}
			zval_dtor(&op1);
			break;
		case ZEND_BOOL_OR:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			if (zend_is_true(&op1)) {
				ZVAL_BOOL(result, 1);
			} else {
				zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
				ZVAL_BOOL(result, zend_is_true(&op2));
				zval_dtor(&op2);
			}
			zval_dtor(&op1);
			break;
		case ZEND_SELECT:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			if (zend_is_true(&op1)) {
				if (!(&ast->u.child)[1]) {
					*result = op1;
				} else {
					zend_ast_evaluate(result, (&ast->u.child)[1], scope TSRMLS_CC);
					zval_dtor(&op1);
				}
			} else {
				zend_ast_evaluate(result, (&ast->u.child)[2], scope TSRMLS_CC);
				zval_dtor(&op1);
			}
			break;
		case ZEND_UNARY_PLUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, (&ast->u.child)[0], scope TSRMLS_CC);
			add_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op2);
			break;
		case ZEND_UNARY_MINUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, (&ast->u.child)[0], scope TSRMLS_CC);
			sub_function(result, &op1, &op2 TSRMLS_CC);
			zval_dtor(&op2);
			break;
		case ZEND_INIT_ARRAY:
			INIT_PZVAL(result);
			array_init(result);
			{
				int i;
				zend_bool has_key;
				for (i = 0; i < ast->children; i+=2) {
					zval *expr;
					MAKE_STD_ZVAL(expr);
					if ((has_key = !!(&ast->u.child)[i])) {
						zend_ast_evaluate(&op1, (&ast->u.child)[i], scope TSRMLS_CC);
					}
					zend_ast_evaluate(expr, (&ast->u.child)[i+1], scope TSRMLS_CC);
					zend_do_add_static_array_element(result, has_key?&op1:NULL, expr);
				}
			}
			break;
		case ZEND_FETCH_DIM_R:
			zend_ast_evaluate(&op1, (&ast->u.child)[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, (&ast->u.child)[1], scope TSRMLS_CC);
			{
				zval *tmp;
				zend_fetch_dimension_by_zval(&tmp, &op1, &op2 TSRMLS_CC);
				ZVAL_ZVAL(result, tmp, 1, 1);
			}
			zval_dtor(&op1);
			zval_dtor(&op2);
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
		zend_ast *copy = zend_ast_create_constant(ast->u.val);
		zval_copy_ctor(copy->u.val);
		return copy;
	} else if (ast->children) {
		zend_ast *new = emalloc(sizeof(zend_ast) + sizeof(zend_ast*) * (ast->children - 1));
		int i;
		new->kind = ast->kind;
		new->children = ast->children;
		for (i = 0; i < ast->children; i++) {
			(&new->u.child)[i] = zend_ast_copy((&ast->u.child)[i]);
		}
		return new;
	}
	return zend_ast_create_dynamic(ast->kind);
}

ZEND_API void zend_ast_destroy(zend_ast *ast)
{
	int i;

	if (ast->kind == ZEND_CONST) {
		zval_dtor(ast->u.val);
	} else {
		for (i = 0; i < ast->children; i++) {
			if ((&ast->u.child)[i]) {
				zend_ast_destroy((&ast->u.child)[i]);
			}
		}
	}
	efree(ast);
}
