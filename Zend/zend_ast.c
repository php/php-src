/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
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

ZEND_API zend_ast_process_t zend_ast_process = NULL;

static inline void *zend_ast_alloc(size_t size) {
	return zend_arena_alloc(&CG(ast_arena), size);
}

static inline void *zend_ast_realloc(void *old, size_t old_size, size_t new_size) {
	void *new = zend_ast_alloc(new_size);
	memcpy(new, old, old_size);
	return new;
}

static inline size_t zend_ast_size(uint32_t children) {
	return sizeof(zend_ast) - sizeof(zend_ast *) + sizeof(zend_ast *) * children;
}

static inline size_t zend_ast_list_size(uint32_t children) {
	return sizeof(zend_ast_list) - sizeof(zend_ast *) + sizeof(zend_ast *) * children;
}

ZEND_API zend_ast *zend_ast_create_znode(znode *node) {
	zend_ast_znode *ast;

	ast = zend_ast_alloc(sizeof(zend_ast_znode));
	ast->kind = ZEND_AST_ZNODE;
	ast->attr = 0;
	ast->lineno = CG(zend_lineno);
	ast->node = *node;
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_zval_ex(zval *zv, zend_ast_attr attr) {
	zend_ast_zval *ast;

	ast = zend_ast_alloc(sizeof(zend_ast_zval));
	ast->kind = ZEND_AST_ZVAL;
	ast->attr = attr;
	ZVAL_COPY_VALUE(&ast->val, zv);
	ast->val.u2.lineno = CG(zend_lineno);
	return (zend_ast *) ast;
}

ZEND_API zend_ast *zend_ast_create_decl(
	zend_ast_kind kind, uint32_t flags, uint32_t start_lineno, zend_string *doc_comment,
	zend_string *name, zend_ast *child0, zend_ast *child1, zend_ast *child2, zend_ast *child3
) {
	zend_ast_decl *ast;

	ast = zend_ast_alloc(sizeof(zend_ast_decl));
	ast->kind = kind;
	ast->attr = 0;
	ast->start_lineno = start_lineno;
	ast->end_lineno = CG(zend_lineno);
	ast->flags = flags;
	ast->lex_pos = LANG_SCNG(yy_text);
	ast->doc_comment = doc_comment;
	ast->name = name;
	ast->child[0] = child0;
	ast->child[1] = child1;
	ast->child[2] = child2;
	ast->child[3] = child3;

	return (zend_ast *) ast;
}

static zend_ast *zend_ast_create_from_va_list(zend_ast_kind kind, zend_ast_attr attr, va_list va) {
	uint32_t i, children = kind >> ZEND_AST_NUM_CHILDREN_SHIFT;
	zend_ast *ast;

	ast = zend_ast_alloc(zend_ast_size(children));
	ast->kind = kind;
	ast->attr = attr;
	ast->lineno = (uint32_t) -1;

	for (i = 0; i < children; ++i) {
		ast->child[i] = va_arg(va, zend_ast *);
		if (ast->child[i] != NULL) {
			uint32_t lineno = zend_ast_get_lineno(ast->child[i]);
			if (lineno < ast->lineno) {
				ast->lineno = lineno;
			}
		}
	}

	if (ast->lineno == UINT_MAX) {
		ast->lineno = CG(zend_lineno);
	}

	return ast;
}

ZEND_API zend_ast *zend_ast_create_ex(zend_ast_kind kind, zend_ast_attr attr, ...) {
	va_list va;
	zend_ast *ast;

	va_start(va, attr);
	ast = zend_ast_create_from_va_list(kind, attr, va);
	va_end(va);

	return ast;
}

ZEND_API zend_ast *zend_ast_create(zend_ast_kind kind, ...) {
	va_list va;
	zend_ast *ast;

	va_start(va, kind);
	ast = zend_ast_create_from_va_list(kind, 0, va);
	va_end(va);

	return ast;
}

ZEND_API zend_ast *zend_ast_create_list(uint32_t init_children, zend_ast_kind kind, ...) {
	zend_ast *ast;
	zend_ast_list *list;

	ast = zend_ast_alloc(zend_ast_list_size(4));
	list = (zend_ast_list *) ast;
	list->kind = kind;
	list->attr = 0;
	list->lineno = CG(zend_lineno);
	list->children = 0;

	{
		va_list va;
		uint32_t i;
		va_start(va, kind);
		for (i = 0; i < init_children; ++i) {
			ast = zend_ast_list_add(ast, va_arg(va, zend_ast *));
		}
		va_end(va);
	}

	return ast;
}

static inline zend_bool is_power_of_two(uint32_t n) {
	return ((n != 0) && (n == (n & (~n + 1))));
}

ZEND_API zend_ast *zend_ast_list_add(zend_ast *ast, zend_ast *op) {
	zend_ast_list *list = zend_ast_get_list(ast);
	if (list->children >= 4 && is_power_of_two(list->children)) {
			list = zend_ast_realloc(list,
			zend_ast_list_size(list->children), zend_ast_list_size(list->children * 2));
	}
	list->child[list->children++] = op;
	return (zend_ast *) list;
}

static void zend_ast_add_array_element(zval *result, zval *offset, zval *expr)
{
	switch (Z_TYPE_P(offset)) {
		case IS_UNDEF:
			zend_hash_next_index_insert(Z_ARRVAL_P(result), expr);
			break;
		case IS_STRING:
			zend_symtable_update(Z_ARRVAL_P(result), Z_STR_P(offset), expr);
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

ZEND_API void zend_ast_evaluate(zval *result, zend_ast *ast, zend_class_entry *scope)
{
	zval op1, op2;

	switch (ast->kind) {
		case ZEND_AST_BINARY_OP:
		{
			binary_op_type op = get_binary_op(ast->attr);
			zend_ast_evaluate(&op1, ast->child[0], scope);
			zend_ast_evaluate(&op2, ast->child[1], scope);
			op(result, &op1, &op2);
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
			zend_ast_evaluate(&op1, ast->child[0], scope);
			zend_ast_evaluate(&op2, ast->child[1], scope);
			op(result, &op2, &op1);
			zval_dtor(&op1);
			zval_dtor(&op2);
			break;
		}
		case ZEND_AST_UNARY_OP:
		{
			unary_op_type op = get_unary_op(ast->attr);
			zend_ast_evaluate(&op1, ast->child[0], scope);
			op(result, &op1);
			zval_dtor(&op1);
			break;
		}
		case ZEND_AST_ZVAL:
		{
			zval *zv = zend_ast_get_zval(ast);
			if (scope) {
				/* class constants may be updated in-place */
				if (Z_OPT_CONSTANT_P(zv)) {
					zval_update_constant_ex(zv, 1, scope);
				}
				ZVAL_DUP(result, zv);
			} else {
				ZVAL_DUP(result, zv);
				if (Z_OPT_CONSTANT_P(result)) {
					zval_update_constant_ex(result, 1, scope);
				}
			}
			break;
		}
		case ZEND_AST_AND:
			zend_ast_evaluate(&op1, ast->child[0], scope);
			if (zend_is_true(&op1)) {
				zend_ast_evaluate(&op2, ast->child[1], scope);
				ZVAL_BOOL(result, zend_is_true(&op2));
				zval_dtor(&op2);
			} else {
				ZVAL_FALSE(result);
			}
			zval_dtor(&op1);
			break;
		case ZEND_AST_OR:
			zend_ast_evaluate(&op1, ast->child[0], scope);
			if (zend_is_true(&op1)) {
				ZVAL_TRUE(result);
			} else {
				zend_ast_evaluate(&op2, ast->child[1], scope);
				ZVAL_BOOL(result, zend_is_true(&op2));
				zval_dtor(&op2);
			}
			zval_dtor(&op1);
			break;
		case ZEND_AST_CONDITIONAL:
			zend_ast_evaluate(&op1, ast->child[0], scope);
			if (zend_is_true(&op1)) {
				if (!ast->child[1]) {
					*result = op1;
				} else {
					zend_ast_evaluate(result, ast->child[1], scope);
					zval_dtor(&op1);
				}
			} else {
				zend_ast_evaluate(result, ast->child[2], scope);
				zval_dtor(&op1);
			}
			break;
		case ZEND_AST_UNARY_PLUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, ast->child[0], scope);
			add_function(result, &op1, &op2);
			zval_dtor(&op2);
			break;
		case ZEND_AST_UNARY_MINUS:
			ZVAL_LONG(&op1, 0);
			zend_ast_evaluate(&op2, ast->child[0], scope);
			sub_function(result, &op1, &op2);
			zval_dtor(&op2);
			break;
		case ZEND_AST_ARRAY:
			array_init(result);
			{
				uint32_t i;
				zend_ast_list *list = zend_ast_get_list(ast);
				for (i = 0; i < list->children; i++) {
					zend_ast *elem = list->child[i];
					if (elem->child[1]) {
						zend_ast_evaluate(&op1, elem->child[1], scope);
					} else {
						ZVAL_UNDEF(&op1);
					}
					zend_ast_evaluate(&op2, elem->child[0], scope);
					zend_ast_add_array_element(result, &op1, &op2);
				}
			}
			break;
		case ZEND_AST_DIM:
			zend_ast_evaluate(&op1, ast->child[0], scope);
			zend_ast_evaluate(&op2, ast->child[1], scope);
			{
				zval tmp;
				zend_fetch_dimension_by_zval(&tmp, &op1, &op2);
				ZVAL_ZVAL(result, &tmp, 1, 1);
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
	} else if (ast->kind == ZEND_AST_ZVAL) {
		zend_ast_zval *new = emalloc(sizeof(zend_ast_zval));
		new->kind = ZEND_AST_ZVAL;
		new->attr = ast->attr;
		ZVAL_COPY(&new->val, zend_ast_get_zval(ast));
		return (zend_ast *) new;
	} else if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		zend_ast_list *new = emalloc(zend_ast_list_size(list->children));
		uint32_t i;
		new->kind = list->kind;
		new->attr = list->attr;
		new->children = list->children;
		for (i = 0; i < list->children; i++) {
			new->child[i] = zend_ast_copy(list->child[i]);
		}
		return (zend_ast *) new;
	} else {
		uint32_t i, children = zend_ast_get_num_children(ast);
		zend_ast *new = emalloc(zend_ast_size(children));
		new->kind = ast->kind;
		new->attr = ast->attr;
		for (i = 0; i < children; i++) {
			new->child[i] = zend_ast_copy(ast->child[i]);
		}
		return new;
	}
}

static void zend_ast_destroy_ex(zend_ast *ast, zend_bool free) {
	if (!ast) {
		return;
	}

	switch (ast->kind) {
		case ZEND_AST_ZVAL:
			/* Destroy value without using GC: When opcache moves arrays into SHM it will
			 * free the zend_array structure, so references to it from outside the op array
			 * become invalid. GC would cause such a reference in the root buffer. */
			zval_ptr_dtor_nogc(zend_ast_get_zval(ast));
			break;
		case ZEND_AST_FUNC_DECL:
		case ZEND_AST_CLOSURE:
		case ZEND_AST_METHOD:
		case ZEND_AST_CLASS:
		{
			zend_ast_decl *decl = (zend_ast_decl *) ast;
			zend_string_release(decl->name);
			if (decl->doc_comment) {
				zend_string_release(decl->doc_comment);
			}
			zend_ast_destroy_ex(decl->child[0], free);
			zend_ast_destroy_ex(decl->child[1], free);
			zend_ast_destroy_ex(decl->child[2], free);
			zend_ast_destroy_ex(decl->child[3], free);
			break;
		}
		default:
			if (zend_ast_is_list(ast)) {
				zend_ast_list *list = zend_ast_get_list(ast);
				uint32_t i;
				for (i = 0; i < list->children; i++) {
					zend_ast_destroy_ex(list->child[i], free);
				}
			} else {
				uint32_t i, children = zend_ast_get_num_children(ast);
				for (i = 0; i < children; i++) {
					zend_ast_destroy_ex(ast->child[i], free);
				}
			}
	}

	if (free) {
		efree(ast);
	}
}

ZEND_API void zend_ast_destroy(zend_ast *ast) {
	zend_ast_destroy_ex(ast, 0);
}
ZEND_API void zend_ast_destroy_and_free(zend_ast *ast) {
	zend_ast_destroy_ex(ast, 1);
}

ZEND_API void zend_ast_apply(zend_ast *ast, zend_ast_apply_func fn) {
	if (zend_ast_is_list(ast)) {
		zend_ast_list *list = zend_ast_get_list(ast);
		uint32_t i;
		for (i = 0; i < list->children; ++i) {
			fn(&list->child[i]);
		}
	} else {
		uint32_t i, children = zend_ast_get_num_children(ast);
		for (i = 0; i < children; ++i) {
			fn(&ast->child[i]);
		}
	}
}
