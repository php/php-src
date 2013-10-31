/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
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
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend_ast.h"
#include "zend_execute.h"

#define OP_IS_CONST_THEN(op, do_code) \
	switch (Z_TYPE_P(op) & IS_CONSTANT_TYPE_MASK) { \
		case IS_CONSTANT: \
		case IS_CONSTANT_ARRAY: \
		case IS_CONSTANT_AST: { \
			do_code \
		} \
	}

#define OP_IS_NOT_CONST_THEN(op, do_code) \
	switch (Z_TYPE_P(op) & IS_CONSTANT_TYPE_MASK) { \
		case IS_CONSTANT: \
		case IS_CONSTANT_ARRAY: \
		case IS_CONSTANT_AST: \
			break; \
\
		default: { \
			do_code \
		} \
	}

#define COPY_ZVAL_TO_OP(nr) \
	Z_AST_P(result)->ops[nr] = emalloc(sizeof(zval)); \
	*Z_AST_P(result)->ops[nr] = *op##nr;

void zend_ast_add(zval *result, intermediary_ast_function_type func, char op_count) {
	zend_ast *ast = emalloc(sizeof(zend_ast));
	ast->op_count = op_count;
	ast->ops = emalloc(op_count * sizeof(zval *));
	ast->refcount = 1;
	ast->func = func;
	Z_AST_P(result) = ast;
	Z_TYPE_P(result) = IS_CONSTANT_AST;
}

/* Do operations on constant operators at compile time (AST building time) */

void zend_ast_add_unary(zval *result, unary_ast_func func, zval *op0) {
	OP_IS_NOT_CONST_THEN(op0,
		func(result, op0);
		return;
	)

	zend_ast_add(result, (intermediary_ast_function_type)func, 1);
	COPY_ZVAL_TO_OP(0)
}

void zend_ast_add_binary(zval *result, binary_ast_func func, zval *op0, zval *op1) {
	OP_IS_NOT_CONST_THEN(op0, OP_IS_NOT_CONST_THEN(op1,
		func(result, op0, op1);
		return;
	))
	
	zend_ast_add(result, (intermediary_ast_function_type)func, 2);
	COPY_ZVAL_TO_OP(0)
	COPY_ZVAL_TO_OP(1)
}

void zend_ast_add_ternary(zval *result, ternary_ast_func func, zval *op0, zval *op1, zval *op2) {
	OP_IS_NOT_CONST_THEN(op0, OP_IS_NOT_CONST_THEN(op1, OP_IS_NOT_CONST_THEN(op2, 
		func(result, op0, op1, op2);
		return;
	)))
	
	zend_ast_add(result, (intermediary_ast_function_type)func, 3);
	COPY_ZVAL_TO_OP(0)
	COPY_ZVAL_TO_OP(1)
	COPY_ZVAL_TO_OP(2)
}

void zend_ast_evaluate(zval *result, zend_ast *ast TSRMLS_DC) {
	int i;

	for (i = ast->op_count; i--;) {
		OP_IS_CONST_THEN(ast->ops[i],
			zval_update_constant_ex(&ast->ops[i], (void *)1, NULL TSRMLS_CC);
		)
	}

	switch (ast->op_count) {
		case 1:
			((unary_ast_func)ast->func)(result, ast->ops[0]);
			break;
		case 2:
			((binary_ast_func)ast->func)(result, ast->ops[0], ast->ops[1]);
			break;
		case 3:
			((ternary_ast_func)ast->func)(result, ast->ops[0], ast->ops[1], ast->ops[2]);
			break;
	}
}

void zend_ast_destroy(zend_ast *ast TSRMLS_DC) {
	int i;

	for (i = ast->op_count; i--;) {
		if (!Z_DELREF_P(ast->ops[i])) {
			zval_dtor(ast->ops[i]);
			efree(ast->ops[i]);
		}
	}

	efree(ast->ops);
	efree(ast);
}
