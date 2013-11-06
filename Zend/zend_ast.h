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

#ifndef ZEND_AST_H
#define ZEND_AST_H

typedef struct _zend_ast zend_ast;

#include "zend.h"

typedef void(*intermediary_ast_function_type)(zval *, ...);
typedef int(*unary_ast_func)(zval *result, zval *op0 TSRMLS_DC);
typedef int(*binary_ast_func)(zval *result, zval *op0, zval *op1 TSRMLS_DC);
typedef int(*ternary_ast_func)(zval *result, zval *op0, zval *op1, zval *op2 TSRMLS_DC);

struct _zend_ast {
	char op_count;
	zval **ops;
	intermediary_ast_function_type func;
	int refcount;
};

void zend_ast_add_unary(zval *result, unary_ast_func func, zval *op0 TSRMLS_DC);
void zend_ast_add_binary(zval *result, binary_ast_func func, zval *op0, zval *op1 TSRMLS_DC);
void zend_ast_add_ternary(zval *result, ternary_ast_func func, zval *op0, zval *op1, zval *op2 TSRMLS_DC);

void zend_ast_evaluate(zval *result, zend_ast *ast TSRMLS_DC);

void zend_ast_destroy(zend_ast *ast TSRMLS_DC);

#define ZEND_AST_ADD_REF(ast) ++ast->refcount

static inline int ZEND_AST_DEL_REF(zend_ast *ast) {
	if (ast->refcount == 1) {
		TSRMLS_FETCH();

		zend_ast_destroy(ast TSRMLS_CC);
		return 0;
	}
	return --ast->refcount;
}

#endif
