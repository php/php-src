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

#ifndef ZEND_AST_H
#define ZEND_AST_H

#include "zend.h"

typedef enum _zend_ast_kind {
	/* first 256 kinds are reserved for opcodes */
	ZEND_CONST = 256, /* TODO.AST: Split in constant lookup and literal zval */
	ZEND_BOOL_AND,
	ZEND_BOOL_OR,
	ZEND_SELECT,
	ZEND_UNARY_PLUS,
	ZEND_UNARY_MINUS,

	ZEND_AST_ZNODE,

	ZEND_AST_VAR,
	ZEND_AST_DIM,
	ZEND_AST_PROP,
	ZEND_AST_STATIC_PROP,

	ZEND_AST_CALL,
	ZEND_AST_METHOD_CALL,
	ZEND_AST_STATIC_CALL,

	ZEND_AST_ASSIGN,
	ZEND_AST_ASSIGN_REF,
	ZEND_AST_LIST,
	ZEND_AST_GLOBAL,
	ZEND_AST_UNSET,

	ZEND_AST_PARAMS,
	ZEND_AST_UNPACK,

	ZEND_AST_NAME,
	ZEND_AST_NAME_FQ,

	ZEND_AST_AND,
	ZEND_AST_OR,

	ZEND_AST_GREATER,
	ZEND_AST_GREATER_EQUAL,

	ZEND_AST_CAST_NULL,
	ZEND_AST_CAST_BOOL,
	ZEND_AST_CAST_INT,
	ZEND_AST_CAST_DOUBLE,
	ZEND_AST_CAST_STRING,
	ZEND_AST_CAST_ARRAY,
	ZEND_AST_CAST_OBJECT,

	ZEND_AST_CONDITIONAL,
} zend_ast_kind;

struct _zend_ast {
	unsigned short kind;
	unsigned short EA;
	zend_uint children;
	zend_ast *child[1];
};

typedef struct _zend_ast_zval {
	unsigned short kind;
	unsigned short EA;
	zval val;
} zend_ast_zval;

static inline zval *zend_ast_get_zval(zend_ast *ast) {
	return &((zend_ast_zval *) ast)->val;
}

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

static inline zend_ast *zend_ast_create_var(zval *name) {
	return zend_ast_create_unary(ZEND_AST_VAR, zend_ast_create_constant(name));
}

/* Temporary, for porting */
#define AST_COMPILE(res, ast) do { \
	zend_ast *_ast = (ast); \
	zend_compile_expr((res), _ast TSRMLS_CC); \
	zend_ast_destroy(_ast); \
} while (0)
#define AST_COMPILE_VAR(res, ast, type) do { \
	zend_ast *_ast = (ast); \
	zend_compile_var((res), _ast, type TSRMLS_CC); \
	zend_ast_destroy(_ast); \
} while (0)
#define AST_COMPILE_STMT(ast) do { \
	zend_ast *_ast = (ast); \
	zend_compile_stmt(_ast TSRMLS_CC); \
	zend_ast_destroy(_ast); \
} while (0)

#define AST_ZNODE(znode) zend_ast_create_znode((znode))
#define AST_ZVAL(znode) zend_ast_create_constant(&(znode)->u.constant)

#define AC(znode) AST_COMPILE(&znode, znode.u.ast)
#define AZ(znode) ((znode).u.ast = AST_ZNODE(&znode))

#endif
