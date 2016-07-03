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

#ifndef ZEND_AST_H
#define ZEND_AST_H

typedef struct _zend_ast zend_ast;

#include "zend.h"

typedef enum _zend_ast_kind {
	/* first 256 kinds are reserved for opcodes */
	ZEND_CONST = 256,
	ZEND_BOOL_AND,
	ZEND_BOOL_OR,
	ZEND_SELECT,
	ZEND_UNARY_PLUS,
	ZEND_UNARY_MINUS,
} zend_ast_kind;

struct _zend_ast {
	unsigned short kind;
	unsigned short children;
	union {
		zval     *val;
		zend_ast *child;
	} u;
};

ZEND_API zend_ast *zend_ast_create_constant(zval *zv);

ZEND_API zend_ast *zend_ast_create_unary(uint kind, zend_ast *op0);
ZEND_API zend_ast *zend_ast_create_binary(uint kind, zend_ast *op0, zend_ast *op1);
ZEND_API zend_ast *zend_ast_create_ternary(uint kind, zend_ast *op0, zend_ast *op1, zend_ast *op2);
ZEND_API zend_ast* zend_ast_create_dynamic(uint kind);
ZEND_API void zend_ast_dynamic_add(zend_ast **ast, zend_ast *op);
ZEND_API void zend_ast_dynamic_shrink(zend_ast **ast);

ZEND_API int zend_ast_is_ct_constant(zend_ast *ast);

ZEND_API void zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope TSRMLS_DC);

ZEND_API zend_ast *zend_ast_copy(zend_ast *ast);
ZEND_API void zend_ast_destroy(zend_ast *ast);

#endif
