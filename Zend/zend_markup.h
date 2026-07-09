/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Liam Hammett                                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_MARKUP_H
#define ZEND_MARKUP_H

#include "zend_ast.h"

/* The fully-qualified names that Native Markup Expressions
 * lowers into. This is the single home for the markup-to-runtime vocabulary:
 * the parser (Zend/zend_language_parser.y) lowers markup expressions to exactly
 * these symbols, and the ext/html runtime registers exactly them (asserted at
 * MINIT, see PHP_MINIT_FUNCTION(html)).
 *
 * Renaming the markup namespace (e.g. the open `Html\` vs `PHP\Html\` question)
 * is a one-line edit here rather than a hunt across the parser, stub, and
 * runtime. Keep in sync with ext/html/html.stub.php, which declares the classes
 * and functions these names refer to. */

#define ZEND_MARKUP_ELEMENT_FQ        "Html\\Element"
#define ZEND_MARKUP_FRAGMENT_FQ       "Html\\Fragment"
#define ZEND_MARKUP_LAZY_FRAGMENT_FQ  "Html\\LazyFragment"
#define ZEND_MARKUP_RAW_FQ            "Html\\raw"
#define ZEND_MARKUP_COMPONENT_FQ      "Html\\render_component"
#define ZEND_MARKUP_DYNAMIC_FQ        "Html\\render_dynamic"

BEGIN_EXTERN_C()

/* Element vs component classification: uppercase first character,
 * a "\", or a "::" dispatches as a component; anything else is a literal HTML
 * element. The single home for this rule - the compiler applies it to static
 * tag names, and Html\render_dynamic applies it to a dynamic tag's runtime
 * value, so `<$tag>` decides at runtime exactly what `<tag>` decides at
 * compile time. */
ZEND_API bool zend_markup_name_is_component(zend_string *tag);

/* Compile-time text handling, called by the lexer (zend_language_scanner.l) on
 * markup text nodes and literal attribute values. Both live in zend_markup.c. */
zend_string *zend_markup_normalize_text(const char *text, size_t len);
zend_string *zend_markup_decode_entities(zend_string *str);

/* AST lowering, called by the markup grammar actions (zend_language_parser.y).
 * Each returns NULL with a CompileError exception pending on invalid markup
 * (e.g. a mismatched closing tag); the grammar action then YYERRORs. */
zend_ast *zend_ast_create_markup_raw(zend_ast *str);
zend_ast *zend_ast_create_markup_element(zend_ast *name, zend_ast *attrs, zend_ast *children);
zend_ast *zend_ast_create_markup_checked(zend_ast *open, zend_ast *attrs, zend_ast *children, zend_ast *close, bool dynamic);
zend_ast *zend_ast_create_markup_dynamic(zend_ast *name, zend_ast *attrs, zend_ast *children);
zend_ast *zend_ast_create_markup_dynamic_expr(zend_ast *expr, zend_ast *attrs, zend_ast *children);

END_EXTERN_C()

#endif /* ZEND_MARKUP_H */
