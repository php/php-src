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

#define COPY_ZVAL_TO_OP(nr) \
	if (op##nr) { \
		Z_AST_P(result)->ops[nr] = emalloc(sizeof(zval)); \
		*Z_AST_P(result)->ops[nr] = *op##nr; \
	} else { \
		Z_AST_P(result)->ops[nr] = NULL; \
	}

void zend_ast_add(zval *result, intermediary_ast_function_type func, char op_count) {
	zend_ast *ast = emalloc(sizeof(zend_ast) + op_count * sizeof(zval *));
	ast->op_count = op_count;
	ast->ops = (zval **)(ast + 1);
	ast->refcount = 1;
	ast->func = func;
	Z_AST_P(result) = ast;
	Z_TYPE_P(result) = IS_CONSTANT_AST;
}

/* Do operations on constant operators at compile time (AST building time) */

void zend_ast_add_unary(zval *result, unary_ast_func func, zval *op0 TSRMLS_DC) {
	if (!op0 || !IS_CONSTANT_TYPE(Z_TYPE_P(op0))) {
		func(result, op0 TSRMLS_CC);
		if (op0) zval_dtor(op0);
		return;
	}

	zend_ast_add(result, (intermediary_ast_function_type)func, 1);
	COPY_ZVAL_TO_OP(0)
}

void zend_ast_add_binary(zval *result, binary_ast_func func, zval *op0, zval *op1 TSRMLS_DC) {
	if ((!op0 || !IS_CONSTANT_TYPE(Z_TYPE_P(op0))) && (!op1 || !IS_CONSTANT_TYPE(Z_TYPE_P(op1)))) {
		func(result, op0, op1 TSRMLS_CC);
		if (op0) zval_dtor(op0);
		if (op1) zval_dtor(op1);
		return;
	}
	
	zend_ast_add(result, (intermediary_ast_function_type)func, 2);
	COPY_ZVAL_TO_OP(0)
	COPY_ZVAL_TO_OP(1)
}

void zend_ast_add_ternary(zval *result, ternary_ast_func func, zval *op0, zval *op1, zval *op2 TSRMLS_DC) {
	if ((!op0 || !IS_CONSTANT_TYPE(Z_TYPE_P(op0))) && (!op1 || !IS_CONSTANT_TYPE(Z_TYPE_P(op1))) && (!op2 || !IS_CONSTANT_TYPE(Z_TYPE_P(op2)))) { 
		func(result, op0, op1, op2 TSRMLS_CC);
		if (op0) zval_dtor(op0);
		if (op1) zval_dtor(op1);
		if (op2) zval_dtor(op2);
		return;
	}
	
	zend_ast_add(result, (intermediary_ast_function_type)func, 3);
	COPY_ZVAL_TO_OP(0)
	COPY_ZVAL_TO_OP(1)
	COPY_ZVAL_TO_OP(2)
}

void zend_ast_evaluate(zval *result, zend_ast *ast TSRMLS_DC) {
	int i;
	zval **ops = emalloc((sizeof(zval *) + sizeof(zval)) * ast->op_count);

	for (i = ast->op_count; i--;) {
		if (ast->ops[i] && IS_CONSTANT_TYPE(Z_TYPE_P(ast->ops[i]))) {
			ops[i] = ((zval *)(ops + ast->op_count)) + i;
			*ops[i] = *ast->ops[i];
			zval_copy_ctor(ops[i]);
			zval_update_constant_ex(&ops[i], (void *)1, NULL TSRMLS_CC);
		} else {
			ops[i] = ast->ops[i];
		}
	}

	switch (ast->op_count) {
		case 1:
			((unary_ast_func)ast->func)(result, ops[0] TSRMLS_CC);
			break;
		case 2:
			((binary_ast_func)ast->func)(result, ops[0], ops[1] TSRMLS_CC);
			break;
		case 3:
			((ternary_ast_func)ast->func)(result, ops[0], ops[1], ops[2] TSRMLS_CC);
			break;
	}

	for (i = ast->op_count; i--;) {
		if (ast->ops[i] != ops[i]) {
			zval_dtor(ops[i]);
		}
	}
	
	efree(ops);
}

void zend_ast_destroy(zend_ast *ast TSRMLS_DC) {
	int i;

	for (i = ast->op_count; i--;) {
		if (ast->ops[i] && !Z_DELREF_P(ast->ops[i])) {
			zval_dtor(ast->ops[i]);
			efree(ast->ops[i]);
		}
	}

	efree(ast);
}
