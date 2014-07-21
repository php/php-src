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

ZEND_API zend_ast *zend_ast_create_znode(znode *node)
{
	TSRMLS_FETCH();
	zend_ast_znode *ast = emalloc(sizeof(zend_ast_znode));
	ast->kind = ZEND_AST_ZNODE;
	ast->attr = 0;
	ast->lineno = CG(zend_lineno);
	ast->node = *node;
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_zval_ex(zval *zv, zend_ast_attr attr)
{
	TSRMLS_FETCH();
	zend_ast_zval *ast = emalloc(sizeof(zend_ast_zval));
	ast->kind = ZEND_AST_ZVAL;
	ast->attr = attr;
	ast->lineno = CG(zend_lineno);
	ZVAL_COPY_VALUE(&ast->val, zv);
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_decl(
	zend_ast_kind kind, zend_uint flags, zend_uint start_lineno, zend_uint end_lineno,
	unsigned char *lex_pos, zend_string *doc_comment, zend_string *name,
	zend_ast *child0, zend_ast *child1, zend_ast *child2
) {
	zend_ast_decl *ast = emalloc(sizeof(zend_ast_decl));

	ast->kind = kind;
	ast->attr = 0;
	ast->start_lineno = start_lineno;
	ast->end_lineno = end_lineno;
	ast->flags = flags;
	ast->lex_pos = lex_pos;
	ast->doc_comment = doc_comment;
	ast->name = name;
	ast->child[0] = child0;
	ast->child[1] = child1;
	ast->child[2] = child2;

	return (zend_ast *) ast;
}

static zend_ast *zend_ast_create_from_va_list(
	zend_uint children, zend_ast_kind kind, zend_ast_attr attr, va_list va
) {
	TSRMLS_FETCH();
	zend_uint i;
	zend_ast *ast;
	
	ast = emalloc(sizeof(zend_ast) + (children - 1) * sizeof(zend_ast *));
	ast->kind = kind;
	ast->attr = attr;
	ast->lineno = UINT_MAX;
	ast->children = children;

	for (i = 0; i < children; ++i) {
		ast->child[i] = va_arg(va, zend_ast *);
		if (ast->child[i] != NULL && ast->child[i]->lineno < ast->lineno) {
			ast->lineno = ast->child[i]->lineno;
		}
	}

	if (ast->lineno == UINT_MAX) {
		ast->lineno = CG(zend_lineno);
	}

	return ast;
}

ZEND_API zend_ast *zend_ast_create_ex(
	zend_uint children, zend_ast_kind kind, zend_ast_attr attr, ...
) {
	va_list va;
	zend_ast *ast;

	va_start(va, attr);
	ast = zend_ast_create_from_va_list(children, kind, attr, va);
	va_end(va);

	return ast;
}

ZEND_API zend_ast *zend_ast_create(
	zend_uint children, zend_ast_kind kind, ...
) {
	va_list va;
	zend_ast *ast;

	va_start(va, kind);
	ast = zend_ast_create_from_va_list(children, kind, 0, va);
	va_end(va);

	return ast;
}

ZEND_API zend_ast *zend_ast_create_dynamic(zend_ast_kind kind)
{
	/* use 4 children as default */
	TSRMLS_FETCH();
	zend_ast *ast = emalloc(sizeof(zend_ast) + sizeof(zend_ast *) * 3);
	ast->kind = kind;
	ast->attr = 0;
	ast->lineno = CG(zend_lineno);
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

	if (ast->kind == ZEND_AST_ZVAL) {
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
		case ZEND_AST_GREATER:
		case ZEND_AST_GREATER_EQUAL:
		{
			/* op1 > op2 is the same as op2 < op1 */
			binary_op_type op = ast->kind == ZEND_AST_GREATER
				? is_smaller_function : is_smaller_or_equal_function;
			zend_ast_evaluate(&op1, ast->child[0], scope TSRMLS_CC);
			zend_ast_evaluate(&op2, ast->child[1], scope TSRMLS_CC);
			op(result, &op2, &op1 TSRMLS_CC);
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
		case ZEND_AST_ZVAL:
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
	} else if (ast->kind == ZEND_AST_ZVAL) {
		zend_ast *copy = zend_ast_create_zval_ex(zend_ast_get_zval(ast), ast->attr);
		zval_copy_ctor(zend_ast_get_zval(copy));
		return copy;
	} else {
		zend_ast *new = emalloc(sizeof(zend_ast) + sizeof(zend_ast *) * (ast->children - 1));
		int i;
		new->kind = ast->kind;
		new->attr = ast->attr;
		new->children = ast->children;
		for (i = 0; i < ast->children; i++) {
			new->child[i] = zend_ast_copy(ast->child[i]);
		}
		return new;
	}
}

ZEND_API void zend_ast_destroy(zend_ast *ast)
{
	if (!ast) {
		return;
	}

	switch (ast->kind) {
		case ZEND_AST_ZVAL:
			zval_ptr_dtor(zend_ast_get_zval(ast));
			break;
		case ZEND_AST_ZNODE:
			break;
		case ZEND_AST_FUNC_DECL:
		case ZEND_AST_CLOSURE:
		case ZEND_AST_METHOD:
		case ZEND_AST_CLASS:
		{
			zend_ast_decl *decl = (zend_ast_decl *) ast;
			STR_RELEASE(decl->name);
			if (decl->doc_comment) {
				STR_RELEASE(decl->doc_comment);
			}
			zend_ast_destroy(decl->child[0]);
			zend_ast_destroy(decl->child[1]);
			zend_ast_destroy(decl->child[2]);
			break;
		}
		default:
		{
			zend_uint i;
			for (i = 0; i < ast->children; i++) {
				zend_ast_destroy(ast->child[i]);
			}
		}
	}

	efree(ast);
}
