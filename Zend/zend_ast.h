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

enum _zend_ast_kind {
	/* first 256 kinds are reserved for opcodes */
	ZEND_CONST = 256, /* TODO.AST: Split in constant lookup and literal zval */
	ZEND_BOOL_AND,
	ZEND_BOOL_OR,
	ZEND_SELECT,

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

	ZEND_AST_ASSIGN_OP,

	ZEND_AST_BINARY_OP,
	ZEND_AST_GREATER,
	ZEND_AST_GREATER_EQUAL,
	ZEND_AST_AND,
	ZEND_AST_OR,

	ZEND_AST_UNARY_PLUS,
	ZEND_AST_UNARY_MINUS,
	ZEND_AST_CAST,

	ZEND_AST_CONDITIONAL,

	ZEND_AST_EMPTY,
	ZEND_AST_ISSET,

	ZEND_AST_SILENCE,
	ZEND_AST_SHELL_EXEC,
	ZEND_AST_ARRAY,
	ZEND_AST_ARRAY_ELEM,

	ZEND_AST_CONST,
	ZEND_AST_CLASS_CONST,
	ZEND_AST_RESOLVE_CLASS_NAME,

	ZEND_AST_ENCAPS_LIST,
};

typedef unsigned short zend_ast_kind;
typedef unsigned short zend_ast_attr;

struct _zend_ast {
	zend_ast_kind kind; /* Type of the node (either opcode or ZEND_AST_* constant) */
	zend_ast_attr attr; /* Additional attribute, use depending on node type */
	zend_uint children; /* Number of children */
	zend_ast *child[1]; /* Array of children (using struct hack) */
};

typedef struct _zend_ast_zval {
	zend_ast_kind kind;
	zend_ast_attr attr;
	zval val;
} zend_ast_zval;

static inline zval *zend_ast_get_zval(zend_ast *ast) {
	return &((zend_ast_zval *) ast)->val;
}

ZEND_API zend_ast *zend_ast_create_constant(zval *zv);

ZEND_API zend_ast *zend_ast_create_zval_ex(zval *zv, zend_ast_attr attr);

ZEND_API zend_ast *zend_ast_create_unary_ex(
	zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0);
ZEND_API zend_ast *zend_ast_create_binary_ex(
	zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0, zend_ast *op1);
ZEND_API zend_ast *zend_ast_create_ternary_ex(
	zend_ast_kind kind, zend_ast_attr attr, zend_ast *op0, zend_ast *op1, zend_ast *op2);

ZEND_API zend_ast *zend_ast_create_dynamic(zend_ast_kind kind);
ZEND_API zend_ast *zend_ast_dynamic_add(zend_ast *ast, zend_ast *op);
ZEND_API void zend_ast_dynamic_shrink(zend_ast **ast);

ZEND_API int zend_ast_is_ct_constant(zend_ast *ast);

ZEND_API void zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope TSRMLS_DC);

ZEND_API zend_ast *zend_ast_copy(zend_ast *ast);
ZEND_API void zend_ast_destroy(zend_ast *ast);

static inline zend_ast *zend_ast_create_zval(zval *zv) {
	return zend_ast_create_zval_ex(zv, 0);
}
static inline zend_ast *zend_ast_create_unary(zend_ast_kind kind, zend_ast *op0) {
	return zend_ast_create_unary_ex(kind, 0, op0);
}
static inline zend_ast *zend_ast_create_binary(zend_ast_kind kind, zend_ast *op0, zend_ast *op1) {
	return zend_ast_create_binary_ex(kind, 0, op0, op1);
}
static inline zend_ast *zend_ast_create_ternary(
	zend_ast_kind kind, zend_ast *op0, zend_ast *op1, zend_ast *op2
) {
	return zend_ast_create_ternary_ex(kind, 0, op0, op1, op2);
}

static inline zend_ast *zend_ast_create_dynamic_and_add(zend_ast_kind kind, zend_ast *op) {
	return zend_ast_dynamic_add(zend_ast_create_dynamic(kind), op);
}

static inline zend_ast *zend_ast_create_var(zval *name) {
	return zend_ast_create_unary(ZEND_AST_VAR, zend_ast_create_constant(name));
}
static inline zend_ast *zend_ast_create_binary_op(zend_uint opcode, zend_ast *op0, zend_ast *op1) {
	return zend_ast_create_binary_ex(ZEND_AST_BINARY_OP, opcode, op0, op1);
}
static inline zend_ast *zend_ast_create_assign_op(zend_uint opcode, zend_ast *op0, zend_ast *op1) {
	return zend_ast_create_binary_ex(ZEND_AST_ASSIGN_OP, opcode, op0, op1);
}

/* Temporary, for porting */
#define AST_COMPILE(res, ast) do { \
	zend_ast *_ast = (ast); \
	zend_eval_const_expr(&_ast TSRMLS_CC); \
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
