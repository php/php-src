%{
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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
 * LALR shift/reduce conflicts and how they are resolved:
 *
 * - 2 shift/reduce conflicts due to the dangling elseif/else ambiguity. Solved by shift.
 *
 */


#include "zend_compile.h"
#include "zend.h"
#include "zend_list.h"
#include "zend_globals.h"
#include "zend_API.h"
#include "zend_constants.h"

#define YYSIZE_T size_t
#define yytnamerr zend_yytnamerr
static YYSIZE_T zend_yytnamerr(char*, const char*);

#define YYERROR_VERBOSE
#define YYSTYPE zend_parser_stack_elem

%}

%pure_parser
%expect 2

%code requires {
#ifdef ZTS
# define YYPARSE_PARAM tsrm_ls
# define YYLEX_PARAM tsrm_ls
#endif
}

%token END 0 "end of file"
%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%token T_INCLUDE      "include (T_INCLUDE)"
%token T_INCLUDE_ONCE "include_once (T_INCLUDE_ONCE)"
%token T_EVAL         "eval (T_EVAL)"
%token T_REQUIRE      "require (T_REQUIRE)"
%token T_REQUIRE_ONCE "require_once (T_REQUIRE_ONCE)"
%left ','
%left T_LOGICAL_OR
%token T_LOGICAL_OR   "or (T_LOGICAL_OR)"
%left T_LOGICAL_XOR
%token T_LOGICAL_XOR  "xor (T_LOGICAL_XOR)"
%left T_LOGICAL_AND
%token T_LOGICAL_AND  "and (T_LOGICAL_AND)"
%right T_PRINT
%token T_PRINT        "print (T_PRINT)"
%right T_YIELD
%token T_YIELD        "yield (T_YIELD)"
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL T_POW_EQUAL
%token T_PLUS_EQUAL   "+= (T_PLUS_EQUAL)"
%token T_MINUS_EQUAL  "-= (T_MINUS_EQUAL)"
%token T_MUL_EQUAL    "*= (T_MUL_EQUAL)"
%token T_DIV_EQUAL    "/= (T_DIV_EQUAL)"
%token T_CONCAT_EQUAL ".= (T_CONCAT_EQUAL)"
%token T_MOD_EQUAL    "%= (T_MOD_EQUAL)"
%token T_AND_EQUAL    "&= (T_AND_EQUAL)"
%token T_OR_EQUAL     "|= (T_OR_EQUAL)"
%token T_XOR_EQUAL    "^= (T_XOR_EQUAL)"
%token T_SL_EQUAL     "<<= (T_SL_EQUAL)"
%token T_SR_EQUAL     ">>= (T_SR_EQUAL)"
%left '?' ':'
%left T_BOOLEAN_OR
%token T_BOOLEAN_OR   "|| (T_BOOLEAN_OR)"
%left T_BOOLEAN_AND 
%token T_BOOLEAN_AND  "&& (T_BOOLEAN_AND)"
%left '|'
%left '^'
%left '&'
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL
%token T_IS_EQUAL     "== (T_IS_EQUAL)"
%token T_IS_NOT_EQUAL "!= (T_IS_NOT_EQUAL)"
%token T_IS_IDENTICAL "=== (T_IS_IDENTICAL)"
%token T_IS_NOT_IDENTICAL "!== (T_IS_NOT_IDENTICAL)"
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL
%token T_IS_SMALLER_OR_EQUAL "<= (T_IS_SMALLER_OR_EQUAL)"
%token T_IS_GREATER_OR_EQUAL ">= (T_IS_GREATER_OR_EQUAL)"
%left T_SL T_SR
%token T_SL "<< (T_SL)"
%token T_SR ">> (T_SR)"
%left '+' '-' '.'
%left '*' '/' '%'
%right '!'
%nonassoc T_INSTANCEOF
%token T_INSTANCEOF  "instanceof (T_INSTANCEOF)"
%right '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right T_POW
%token T_INC "++ (T_INC)"
%token T_DEC "-- (T_DEC)"
%token T_INT_CAST    "(int) (T_INT_CAST)"
%token T_DOUBLE_CAST "(double) (T_DOUBLE_CAST)"
%token T_STRING_CAST "(string) (T_STRING_CAST)"
%token T_ARRAY_CAST  "(array) (T_ARRAY_CAST)"
%token T_OBJECT_CAST "(object) (T_OBJECT_CAST)"
%token T_BOOL_CAST   "(bool) (T_BOOL_CAST)"
%token T_UNSET_CAST  "(unset) (T_UNSET_CAST)"
%right '['
%nonassoc T_NEW T_CLONE
%token T_NEW       "new (T_NEW)"
%token T_CLONE     "clone (T_CLONE)"
%token T_EXIT      "exit (T_EXIT)"
%token T_IF        "if (T_IF)"
%left T_ELSEIF
%token T_ELSEIF    "elseif (T_ELSEIF)"
%left T_ELSE 
%token T_ELSE      "else (T_ELSE)"
%left T_ENDIF 
%token T_ENDIF     "endif (T_ENDIF)"
%token T_LNUMBER   "integer number (T_LNUMBER)"
%token T_DNUMBER   "floating-point number (T_DNUMBER)"
%token T_STRING    "identifier (T_STRING)"
%token T_STRING_VARNAME "variable name (T_STRING_VARNAME)"
%token T_VARIABLE  "variable (T_VARIABLE)"
%token T_NUM_STRING "number (T_NUM_STRING)"
%token T_INLINE_HTML
%token T_CHARACTER
%token T_BAD_CHARACTER
%token T_ENCAPSED_AND_WHITESPACE  "quoted-string and whitespace (T_ENCAPSED_AND_WHITESPACE)"
%token T_CONSTANT_ENCAPSED_STRING "quoted-string (T_CONSTANT_ENCAPSED_STRING)"
%token T_ECHO       "echo (T_ECHO)"
%token T_DO         "do (T_DO)"
%token T_WHILE      "while (T_WHILE)"
%token T_ENDWHILE   "endwhile (T_ENDWHILE)"
%token T_FOR        "for (T_FOR)"
%token T_ENDFOR     "endfor (T_ENDFOR)"
%token T_FOREACH    "foreach (T_FOREACH)"
%token T_ENDFOREACH "endforeach (T_ENDFOREACH)"
%token T_DECLARE    "declare (T_DECLARE)"
%token T_ENDDECLARE "enddeclare (T_ENDDECLARE)"
%token T_AS         "as (T_AS)"
%token T_SWITCH     "switch (T_SWITCH)"
%token T_ENDSWITCH  "endswitch (T_ENDSWITCH)"
%token T_CASE       "case (T_CASE)"
%token T_DEFAULT    "default (T_DEFAULT)"
%token T_BREAK      "break (T_BREAK)"
%token T_CONTINUE   "continue (T_CONTINUE)"
%token T_GOTO       "goto (T_GOTO)"
%token T_FUNCTION   "function (T_FUNCTION)"
%token T_CONST      "const (T_CONST)"
%token T_RETURN     "return (T_RETURN)"
%token T_TRY        "try (T_TRY)"
%token T_CATCH      "catch (T_CATCH)"
%token T_FINALLY    "finally (T_FINALLY)"
%token T_THROW      "throw (T_THROW)"
%token T_USE        "use (T_USE)"
%token T_INSTEADOF  "insteadof (T_INSTEADOF)"
%token T_GLOBAL     "global (T_GLOBAL)"
%right T_STATIC T_ABSTRACT T_FINAL T_PRIVATE T_PROTECTED T_PUBLIC
%token T_STATIC     "static (T_STATIC)"
%token T_ABSTRACT   "abstract (T_ABSTRACT)"
%token T_FINAL      "final (T_FINAL)"
%token T_PRIVATE    "private (T_PRIVATE)"
%token T_PROTECTED  "protected (T_PROTECTED)"
%token T_PUBLIC     "public (T_PUBLIC)"
%token T_VAR        "var (T_VAR)"
%token T_UNSET      "unset (T_UNSET)"
%token T_ISSET      "isset (T_ISSET)"
%token T_EMPTY      "empty (T_EMPTY)"
%token T_HALT_COMPILER "__halt_compiler (T_HALT_COMPILER)"
%token T_CLASS      "class (T_CLASS)"
%token T_TRAIT      "trait (T_TRAIT)"
%token T_INTERFACE  "interface (T_INTERFACE)"
%token T_EXTENDS    "extends (T_EXTENDS)"
%token T_IMPLEMENTS "implements (T_IMPLEMENTS)"
%token T_OBJECT_OPERATOR "-> (T_OBJECT_OPERATOR)"
%right T_DOUBLE_ARROW
%token T_DOUBLE_ARROW    "=> (T_DOUBLE_ARROW)"
%token T_LIST            "list (T_LIST)"
%token T_ARRAY           "array (T_ARRAY)"
%token T_CALLABLE        "callable (T_CALLABLE)"
%token T_CLASS_C         "__CLASS__ (T_CLASS_C)"
%token T_TRAIT_C         "__TRAIT__ (T_TRAIT_C)"
%token T_METHOD_C        "__METHOD__ (T_METHOD_C)"
%token T_FUNC_C          "__FUNCTION__ (T_FUNC_C)"
%token T_LINE            "__LINE__ (T_LINE)"
%token T_FILE            "__FILE__ (T_FILE)"
%token T_COMMENT         "comment (T_COMMENT)"
%token T_DOC_COMMENT     "doc comment (T_DOC_COMMENT)"
%token T_OPEN_TAG        "open tag (T_OPEN_TAG)"
%token T_OPEN_TAG_WITH_ECHO "open tag with echo (T_OPEN_TAG_WITH_ECHO)"
%token T_CLOSE_TAG       "close tag (T_CLOSE_TAG)"
%token T_WHITESPACE      "whitespace (T_WHITESPACE)"
%token T_START_HEREDOC   "heredoc start (T_START_HEREDOC)"
%token T_END_HEREDOC     "heredoc end (T_END_HEREDOC)"
%token T_DOLLAR_OPEN_CURLY_BRACES "${ (T_DOLLAR_OPEN_CURLY_BRACES)"
%token T_CURLY_OPEN      "{$ (T_CURLY_OPEN)"
%token T_PAAMAYIM_NEKUDOTAYIM ":: (T_PAAMAYIM_NEKUDOTAYIM)"
%token T_NAMESPACE       "namespace (T_NAMESPACE)"
%token T_NS_C            "__NAMESPACE__ (T_NS_C)"
%token T_DIR             "__DIR__ (T_DIR)"
%token T_NS_SEPARATOR    "\\ (T_NS_SEPARATOR)"
%token T_ELLIPSIS        "... (T_ELLIPSIS)"
%token T_POW             "** (T_POW)"
%token T_POW_EQUAL       "**= (T_POW_EQUAL)"

%% /* Rules */

start:
	top_statement_list	{ CG(ast) = $1.ast; }
;

top_statement_list:
		top_statement_list top_statement { $$.list = zend_ast_list_add($1.list, $2.ast); }
	|	/* empty */ { $$.list = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;

namespace_name:
		T_STRING { $$.ast = $1.ast; }
	|	namespace_name T_NS_SEPARATOR T_STRING
			{ $$.ast = zend_ast_append_str($1.ast, $3.ast); }
;

name:
		namespace_name
			{ $$.ast = $1.ast; $$.ast->attr = ZEND_NAME_NOT_FQ; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name
			{ $$.ast = $3.ast; $$.ast->attr = ZEND_NAME_RELATIVE; }
	|	T_NS_SEPARATOR namespace_name
			{ $$.ast = $2.ast; $$.ast->attr = ZEND_NAME_FQ; }
;

top_statement:
		statement						{ $$.ast = $1.ast; }
	|	function_declaration_statement	{ $$.ast = $1.ast; }
	|	class_declaration_statement		{ $$.ast = $1.ast; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ $$.ast = zend_ast_create_unary(ZEND_AST_HALT_COMPILER,
			      zend_ast_create_zval_from_long(zend_get_scanned_file_offset(TSRMLS_C)));
			  zend_stop_lexing(TSRMLS_C); }
	|	T_NAMESPACE namespace_name ';'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, $2.ast, NULL);
			  zend_discard_doc_comment(TSRMLS_C); }
	|	T_NAMESPACE namespace_name { zend_discard_doc_comment(TSRMLS_C); }
		'{' top_statement_list '}'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, $2.ast, $5.ast); }
	|	T_NAMESPACE { zend_discard_doc_comment(TSRMLS_C); }
		'{' top_statement_list '}'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, NULL, $4.ast); }
	|	T_USE use_declarations ';'
			{ $$.ast = $2.ast; $$.ast->attr = T_CLASS; }
	|	T_USE T_FUNCTION use_declarations ';'
			{ $$.ast = $3.ast; $$.ast->attr = T_FUNCTION; }
	|	T_USE T_CONST use_declarations ';'
			{ $$.ast = $3.ast; $$.ast->attr = T_CONST; }
	|	T_CONST const_list ';'			{ $$.ast = $2.ast; }
;

use_declarations:
		use_declarations ',' use_declaration
			{ $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	use_declaration
			{ $$.list = zend_ast_create_list(1, ZEND_AST_USE, $1.ast); }
;

use_declaration:
		namespace_name
			{ $$.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, $1.ast, NULL); }
	|	namespace_name T_AS T_STRING
			{ $$.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, $1.ast, $3.ast); }
	|	T_NS_SEPARATOR namespace_name
			{ $$.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, $2.ast, NULL); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING
			{ $$.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, $2.ast, $4.ast); }
;

const_list:
		const_list ',' const_decl { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	const_decl { $$.list = zend_ast_create_list(1, ZEND_AST_CONST_DECL, $1.ast); }
;

inner_statement_list:
		inner_statement_list inner_statement
			{ $$.list = zend_ast_list_add($1.list, $2.ast); }
	|	/* empty */
			{ $$.list = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


inner_statement:
		statement { $$.ast = $1.ast; }
	|	function_declaration_statement { $$.ast = $1.ast; }
	|	class_declaration_statement { $$.ast = $1.ast; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ zend_error_noreturn(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
;


statement:
		unticked_statement { $$.ast = $1.ast; }
	|	T_STRING ':' { $$.ast = zend_ast_create_unary(ZEND_AST_LABEL, $1.ast); }
;

unticked_statement:
		'{' inner_statement_list '}' { $$.ast = $2.ast; }
	|	if_stmt { $$.ast = $1.ast; }
	|	alt_if_stmt { $$.ast = $1.ast; }
	|	T_WHILE parenthesis_expr while_statement
			{ $$.ast = zend_ast_create_binary(ZEND_AST_WHILE, $2.ast, $3.ast); }
	|	T_DO statement T_WHILE parenthesis_expr ';'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DO_WHILE, $2.ast, $4.ast); }
	|	T_FOR '(' for_expr ';' for_expr ';' for_expr ')' for_statement
			{ $$.ast = zend_ast_create(4, ZEND_AST_FOR, $3.ast, $5.ast, $7.ast, $9.ast); }
	|	T_SWITCH parenthesis_expr switch_case_list
			{ $$.ast = zend_ast_create_binary(ZEND_AST_SWITCH, $2.ast, $3.ast); }
	|	T_BREAK ';'			{ $$.ast = zend_ast_create_unary(ZEND_AST_BREAK, NULL); }
	|	T_BREAK expr ';'	{ $$.ast = zend_ast_create_unary(ZEND_AST_BREAK, $2.ast); }
	|	T_CONTINUE ';'		{ $$.ast = zend_ast_create_unary(ZEND_AST_CONTINUE, NULL); }
	|	T_CONTINUE expr ';'	{ $$.ast = zend_ast_create_unary(ZEND_AST_CONTINUE, $2.ast); }
	|	T_RETURN ';'
			{ $$.ast = zend_ast_create_unary(ZEND_AST_RETURN, NULL); }
	|	T_RETURN expr ';'
			{ $$.ast = zend_ast_create_unary(ZEND_AST_RETURN, $2.ast); }
	|	T_GLOBAL global_var_list ';' { $$.ast = $2.ast; }
	|	T_STATIC static_var_list ';' { $$.ast = $2.ast; }
	|	T_ECHO echo_expr_list ';' { $$.ast = $2.ast; }
	|	T_INLINE_HTML { $$.ast = zend_ast_create_unary(ZEND_AST_ECHO, $1.ast); }
	|	expr ';' { $$.ast = $1.ast; }
	|	T_UNSET '(' unset_variables ')' ';' { $$.ast = $3.ast; }
	|	T_FOREACH '(' expr T_AS foreach_variable ')' foreach_statement
			{ $$.ast = zend_ast_create(4, ZEND_AST_FOREACH,
			      $3.ast, $5.ast, NULL, $7.ast); }
	|	T_FOREACH '(' expr T_AS foreach_variable T_DOUBLE_ARROW foreach_variable ')'
		foreach_statement
			{ $$.ast = zend_ast_create(4, ZEND_AST_FOREACH,
			      $3.ast, $7.ast, $5.ast, $9.ast); }
	|	T_DECLARE '(' const_list ')' declare_statement
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DECLARE, $3.ast, $5.ast); }
	|	';'	/* empty statement */ { $$.ast = NULL; }
	|	T_TRY '{' inner_statement_list '}' catch_list finally_statement
			{ $$.ast = zend_ast_create_ternary(ZEND_AST_TRY, $3.ast, $5.ast, $6.ast); }
	|	T_THROW expr ';' { $$.ast = zend_ast_create_unary(ZEND_AST_THROW, $2.ast); }
	|	T_GOTO T_STRING ';' { $$.ast = zend_ast_create_unary(ZEND_AST_GOTO, $2.ast); }
;

catch_list:
		/* empty */
			{ $$.list = zend_ast_create_list(0, ZEND_AST_CATCH_LIST); }
	|	catch_list T_CATCH '(' name T_VARIABLE ')' '{' inner_statement_list '}'
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_ternary(ZEND_AST_CATCH, $4.ast, $5.ast, $8.ast)); }
;

finally_statement:
		/* empty */ { $$.ast = NULL; }
	|	T_FINALLY '{' inner_statement_list '}' { $$.ast = $3.ast; }
;

unset_variables:
		unset_variable { $$.list = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1.ast); }
	|	unset_variables ',' unset_variable { $$.list = zend_ast_list_add($1.list, $3.ast); }
;

unset_variable:
		variable { $$.ast = zend_ast_create_unary(ZEND_AST_UNSET, $1.ast); }
;

function_declaration_statement:
	function returns_ref T_STRING '(' parameter_list ')'
		{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }
	'{' inner_statement_list '}'
		{ $$.ast = zend_ast_create_decl(ZEND_AST_FUNC_DECL, $2.num, $1.num, $7.str,
		      zend_ast_get_str($3.ast), $5.ast, NULL, $9.ast); }
;

is_reference:
		/* empty */	{ $$.num = 0; }
	|	'&'			{ $$.num = ZEND_PARAM_REF; }
;

is_variadic:
		/* empty */ { $$.num = 0; }
	|	T_ELLIPSIS  { $$.num = ZEND_PARAM_VARIADIC; }
;

class_declaration_statement:
		class_type
			{ $$.num = CG(zend_lineno); }
		T_STRING extends_from implements_list
			{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }	
		'{' class_statement_list '}'
			{ $$.ast = zend_ast_create_decl(ZEND_AST_CLASS, $1.num, $2.num, $6.str,
				  zend_ast_get_str($3.ast), $4.ast, $5.ast, $8.ast); }
	|	T_INTERFACE
			{ $$.num = CG(zend_lineno); }
		T_STRING interface_extends_list
			{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }	
		'{' class_statement_list '}'
			{ $$.ast = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_INTERFACE, $2.num, $5.str,
				  zend_ast_get_str($3.ast), NULL, $4.ast, $7.ast); }
;

class_type:
		T_CLASS				{ $$.num = 0; }
	|	T_ABSTRACT T_CLASS	{ $$.num = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
	|	T_FINAL T_CLASS		{ $$.num = ZEND_ACC_FINAL_CLASS; }
	|	T_TRAIT				{ $$.num = ZEND_ACC_TRAIT; }
;

extends_from:
		/* empty */		{ $$.ast = NULL; }
	|	T_EXTENDS name	{ $$.ast = $2.ast; }
;

interface_extends_list:
		/* empty */				{ $$.ast = NULL; }
	|	T_EXTENDS name_list	{ $$.ast = $2.ast; }
;

implements_list:
		/* empty */				{ $$.ast = NULL; }
	|	T_IMPLEMENTS name_list	{ $$.ast = $2.ast; }
;

foreach_variable:
		variable			{ $$.ast = $1.ast; }
	|	'&' variable		{ $$.ast = zend_ast_create_unary(ZEND_AST_REF, $2.ast); }
	|	T_LIST '(' assignment_list ')' { $$.ast = $3.ast; }
;

for_statement:
		statement { $$.ast = $1.ast; }
	|	':' inner_statement_list T_ENDFOR ';' { $$.ast = $2.ast; }
;


foreach_statement:
		statement { $$.ast = $1.ast; }
	|	':' inner_statement_list T_ENDFOREACH ';' { $$.ast = $2.ast; }
;


declare_statement:
		statement { $$.ast = $1.ast; }
	|	':' inner_statement_list T_ENDDECLARE ';' { $$.ast = $2.ast; }
;

switch_case_list:
		'{' case_list '}'					{ $$.ast = $2.ast; }
	|	'{' ';' case_list '}'				{ $$.ast = $3.ast; }
	|	':' case_list T_ENDSWITCH ';'		{ $$.ast = $2.ast; }
	|	':' ';' case_list T_ENDSWITCH ';'	{ $$.ast = $3.ast; }
;

case_list:
		/* empty */ { $$.list = zend_ast_create_list(0, ZEND_AST_SWITCH_LIST); }
	|	case_list T_CASE expr case_separator inner_statement_list
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_SWITCH_CASE, $3.ast, $5.ast)); }
	|	case_list T_DEFAULT case_separator inner_statement_list
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_SWITCH_CASE, NULL, $4.ast)); }
;

case_separator:
		':'
	|	';'
;


while_statement:
		statement { $$.ast = $1.ast; }
	|	':' inner_statement_list T_ENDWHILE ';' { $$.ast = $2.ast; }
;


if_stmt_without_else:
		T_IF parenthesis_expr statement
			{ $$.list = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $2.ast, $3.ast)); }
	|	if_stmt_without_else T_ELSEIF parenthesis_expr statement
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $3.ast, $4.ast)); }
;

if_stmt:
		if_stmt_without_else { $$.ast = $1.ast; }
	|	if_stmt_without_else T_ELSE statement
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, NULL, $3.ast)); }
;

alt_if_stmt_without_else:
		T_IF parenthesis_expr ':' inner_statement_list
			{ $$.list = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $2.ast, $4.ast)); }
	|	alt_if_stmt_without_else T_ELSEIF parenthesis_expr ':' inner_statement_list
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $3.ast, $5.ast)); }
;

alt_if_stmt:
		alt_if_stmt_without_else T_ENDIF ';' { $$.ast = $1.ast; }
	|	alt_if_stmt_without_else T_ELSE ':' inner_statement_list T_ENDIF ';'
			{ $$.list = zend_ast_list_add($1.list,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, NULL, $4.ast)); }
;

parameter_list:
		non_empty_parameter_list { $$.ast = $1.ast; }
	|	/* empty */	{ $$.list = zend_ast_create_list(0, ZEND_AST_PARAM_LIST); }
;


non_empty_parameter_list:
		parameter
			{ $$.list = zend_ast_create_list(1, ZEND_AST_PARAM_LIST, $1.ast); }
	|	non_empty_parameter_list ',' parameter
			{ $$.list = zend_ast_list_add($1.list, $3.ast); }
;

parameter:
		optional_type is_reference is_variadic T_VARIABLE
			{ $$.ast = zend_ast_create_ex(3, ZEND_AST_PARAM, $2.num | $3.num,
			      $1.ast, $4.ast, NULL); }
	|	optional_type is_reference is_variadic T_VARIABLE '=' expr
			{ $$.ast = zend_ast_create_ex(3, ZEND_AST_PARAM, $2.num | $3.num,
			      $1.ast, $4.ast, $6.ast); }
;


optional_type:
		/* empty */	{ $$.ast = NULL; }
	|	T_ARRAY		{ $$.ast = zend_ast_create_ex(0, ZEND_AST_TYPE, IS_ARRAY); }
	|	T_CALLABLE	{ $$.ast = zend_ast_create_ex(0, ZEND_AST_TYPE, IS_CALLABLE); }
	|	name		{ $$.ast = $1.ast; }
;

argument_list:
		'(' ')'	{ $$.list = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	'(' non_empty_argument_list ')' { $$.ast = $2.ast; }
;

non_empty_argument_list:
		argument
			{ $$.list = zend_ast_create_list(1, ZEND_AST_ARG_LIST, $1.ast); }
	|	non_empty_argument_list ',' argument
			{ $$.list = zend_ast_list_add($1.list, $3.ast); }
;

argument:
		expr_without_variable	{ $$.ast = $1.ast; }
	|	variable				{ $$.ast = $1.ast; }
	/*|	'&' variable 			{ ZEND_ASSERT(0); } */
	|	T_ELLIPSIS expr			{ $$.ast = zend_ast_create_unary(ZEND_AST_UNPACK, $2.ast); }
;

global_var_list:
		global_var_list ',' global_var { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	global_var { $$.list = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1.ast); }
;

global_var:
	simple_variable
		{ $$.ast = zend_ast_create_unary(ZEND_AST_GLOBAL,
		      zend_ast_create_unary(ZEND_AST_VAR, $1.ast)); }
;


static_var_list:
		static_var_list ',' static_var { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	static_var { $$.list = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1.ast); }
;

static_var:
		T_VARIABLE
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC, $1.ast, NULL); }
	|	T_VARIABLE '=' expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC, $1.ast, $3.ast); }
;


class_statement_list:
		class_statement_list class_statement
			{ $$.list = zend_ast_list_add($1.list, $2.ast); }
	|	/* empty */
			{ $$.list = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


class_statement:
		variable_modifiers property_list ';'
			{ $$.ast = $2.ast; $$.ast->attr = $1.num; }
	|	T_CONST class_const_list ';'
			{ $$.ast = $2.ast; zend_discard_doc_comment(TSRMLS_C); }
	|	T_USE name_list trait_adaptations
			{ $$.ast = zend_ast_create_binary(ZEND_AST_USE_TRAIT, $2.ast, $3.ast); }
	|	method_modifiers function returns_ref T_STRING '(' parameter_list ')'
			{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }
		method_body
			{ $$.ast = zend_ast_create_decl(ZEND_AST_METHOD, $3.num | $1.num, $2.num, $8.str,
				  zend_ast_get_str($4.ast), $6.ast, NULL, $9.ast); }
;

name_list:
		name { $$.list = zend_ast_create_list(1, ZEND_AST_NAME_LIST, $1.ast); }
	|	name_list ',' name { $$.list = zend_ast_list_add($1.list, $3.ast); }
;

trait_adaptations:
		';'								{ $$.ast = NULL; }
	|	'{' '}'							{ $$.ast = NULL; }
	|	'{' trait_adaptation_list '}'	{ $$.ast = $2.ast; }
;

trait_adaptation_list:
		trait_adaptation
			{ $$.list = zend_ast_create_list(1, ZEND_AST_TRAIT_ADAPTATIONS, $1.ast); }
	|	trait_adaptation_list trait_adaptation
			{ $$.list = zend_ast_list_add($1.list, $2.ast); }
;

trait_adaptation:
		trait_precedence ';'	{ $$.ast = $1.ast; }
	|	trait_alias ';'			{ $$.ast = $1.ast; }
;

trait_precedence:
	absolute_trait_method_reference T_INSTEADOF name_list
		{ $$.ast = zend_ast_create_binary(ZEND_AST_TRAIT_PRECEDENCE, $1.ast, $3.ast); }
;

trait_alias:
		trait_method_reference T_AS trait_modifiers T_STRING
			{ $$.ast = zend_ast_create_ex(2, ZEND_AST_TRAIT_ALIAS,
			      $3.num, $1.ast, $4.ast); }
	|	trait_method_reference T_AS member_modifier
			{ $$.ast = zend_ast_create_ex(2, ZEND_AST_TRAIT_ALIAS,
			      $3.num, $1.ast, NULL); }
;

trait_method_reference:
		T_STRING
			{ $$.ast = zend_ast_create_binary(ZEND_AST_METHOD_REFERENCE, NULL, $1.ast); }
	|	absolute_trait_method_reference { $$.ast = $1.ast; }
;

absolute_trait_method_reference:
	name T_PAAMAYIM_NEKUDOTAYIM T_STRING
		{ $$.ast = zend_ast_create_binary(ZEND_AST_METHOD_REFERENCE, $1.ast, $3.ast); }
;

trait_modifiers:
		/* empty */		{ $$.num = 0; }
	|	member_modifier	{ $$.num = $1.num; }
;

method_body:
		';' /* abstract method */		{ $$.ast = NULL; }
	|	'{' inner_statement_list '}'	{ $$.ast = $2.ast; }
;

variable_modifiers:
		non_empty_member_modifiers		{ $$.num = $1.num; }
	|	T_VAR							{ $$.num = ZEND_ACC_PUBLIC; }
;

method_modifiers:
		/* empty */						{ $$.num = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers
			{ $$.num = $1.num; if (!($$.num & ZEND_ACC_PPP_MASK)) { $$.num |= ZEND_ACC_PUBLIC; } }
;

non_empty_member_modifiers:
		member_modifier			{ $$.num = $1.num; }
	|	non_empty_member_modifiers member_modifier
			{ $$.num = zend_add_member_modifier($1.num, $2.num); }
;

member_modifier:
		T_PUBLIC				{ $$.num = ZEND_ACC_PUBLIC; }
	|	T_PROTECTED				{ $$.num = ZEND_ACC_PROTECTED; }
	|	T_PRIVATE				{ $$.num = ZEND_ACC_PRIVATE; }
	|	T_STATIC				{ $$.num = ZEND_ACC_STATIC; }
	|	T_ABSTRACT				{ $$.num = ZEND_ACC_ABSTRACT; }
	|	T_FINAL					{ $$.num = ZEND_ACC_FINAL; }
;

property_list:
		property_list ',' property { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	property { $$.list = zend_ast_create_list(1, ZEND_AST_PROP_DECL, $1.ast); }
;

property:
		T_VARIABLE { $$.ast = zend_ast_create_binary(ZEND_AST_PROP_ELEM, $1.ast, NULL); }
	|	T_VARIABLE '=' expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_PROP_ELEM, $1.ast, $3.ast); }
;

class_const_list:
		class_const_list ',' const_decl { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	const_decl
			{ $$.list = zend_ast_create_list(1, ZEND_AST_CLASS_CONST_DECL, $1.ast); }
;

const_decl:
	T_STRING '=' expr
		{ $$.ast = zend_ast_create_binary(ZEND_AST_CONST_ELEM, $1.ast, $3.ast); }
;

echo_expr_list:
		echo_expr_list ',' echo_expr { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	echo_expr { $$.list = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1.ast); }
;
echo_expr:
	expr { $$.ast = zend_ast_create_unary(ZEND_AST_ECHO, $1.ast); }
;

for_expr:
		/* empty */			{ $$.ast = NULL; }
	|	non_empty_for_expr	{ $$.ast = $1.ast; }
;

non_empty_for_expr:
		non_empty_for_expr ',' expr { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	expr { $$.list = zend_ast_create_list(1, ZEND_AST_EXPR_LIST, $1.ast); }
;

new_expr:
		T_NEW class_name_reference ctor_arguments
			{ $$.ast = zend_ast_create_binary(ZEND_AST_NEW, $2.ast, $3.ast); }
;

expr_without_variable:
		T_LIST '(' assignment_list ')' '=' expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_ASSIGN, $3.ast, $6.ast); }
	|	variable '=' expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_ASSIGN, $1.ast, $3.ast); }
	|	variable '=' '&' variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_ASSIGN_REF, $1.ast, $4.ast); }
	|	variable '=' '&' T_NEW class_name_reference ctor_arguments
			{ zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");
			  $$.ast = zend_ast_create_binary(ZEND_AST_ASSIGN_REF, $1.ast,
			      zend_ast_create_binary(ZEND_AST_NEW, $5.ast, $6.ast)); }
	|	T_CLONE expr { $$.ast = zend_ast_create_unary(ZEND_AST_CLONE, $2.ast); }
	|	variable T_PLUS_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_ADD, $1.ast, $3.ast); }
	|	variable T_MINUS_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SUB, $1.ast, $3.ast); }
	|	variable T_MUL_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_MUL, $1.ast, $3.ast); }
	|	variable T_POW_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_POW, $1.ast, $3.ast); }
	|	variable T_DIV_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_DIV, $1.ast, $3.ast); }
	|	variable T_CONCAT_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_CONCAT, $1.ast, $3.ast); }
	|	variable T_MOD_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_MOD, $1.ast, $3.ast); }
	|	variable T_AND_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_AND, $1.ast, $3.ast); }
	|	variable T_OR_EQUAL expr 
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_OR, $1.ast, $3.ast); }
	|	variable T_XOR_EQUAL expr 
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_XOR, $1.ast, $3.ast); }
	|	variable T_SL_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SL, $1.ast, $3.ast); }
	|	variable T_SR_EQUAL expr
			{ $$.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SR, $1.ast, $3.ast); }
	|	variable T_INC { $$.ast = zend_ast_create_unary(ZEND_AST_POST_INC, $1.ast); }
	|	T_INC variable { $$.ast = zend_ast_create_unary(ZEND_AST_PRE_INC, $2.ast); }
	|	variable T_DEC { $$.ast = zend_ast_create_unary(ZEND_AST_POST_DEC, $1.ast); }
	|	T_DEC variable { $$.ast = zend_ast_create_unary(ZEND_AST_PRE_DEC, $2.ast); }
	|	expr T_BOOLEAN_OR expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_OR, $1.ast, $3.ast); }
	|	expr T_BOOLEAN_AND expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_AND, $1.ast, $3.ast); }
	|	expr T_LOGICAL_OR expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_OR, $1.ast, $3.ast); }
	|	expr T_LOGICAL_AND expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_AND, $1.ast, $3.ast); }
	|	expr T_LOGICAL_XOR expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_BOOL_XOR, $1.ast, $3.ast); }
	|	expr '|' expr	{ $$.ast = zend_ast_create_binary_op(ZEND_BW_OR, $1.ast, $3.ast); }
	|	expr '&' expr	{ $$.ast = zend_ast_create_binary_op(ZEND_BW_AND, $1.ast, $3.ast); }
	|	expr '^' expr	{ $$.ast = zend_ast_create_binary_op(ZEND_BW_XOR, $1.ast, $3.ast); }
	|	expr '.' expr 	{ $$.ast = zend_ast_create_binary_op(ZEND_CONCAT, $1.ast, $3.ast); }
	|	expr '+' expr 	{ $$.ast = zend_ast_create_binary_op(ZEND_ADD, $1.ast, $3.ast); }
	|	expr '-' expr 	{ $$.ast = zend_ast_create_binary_op(ZEND_SUB, $1.ast, $3.ast); }
	|	expr '*' expr	{ $$.ast = zend_ast_create_binary_op(ZEND_MUL, $1.ast, $3.ast); }
	|	expr T_POW expr	{ $$.ast = zend_ast_create_binary_op(ZEND_POW, $1.ast, $3.ast); }
	|	expr '/' expr	{ $$.ast = zend_ast_create_binary_op(ZEND_DIV, $1.ast, $3.ast); }
	|	expr '%' expr 	{ $$.ast = zend_ast_create_binary_op(ZEND_MOD, $1.ast, $3.ast); }
	| 	expr T_SL expr	{ $$.ast = zend_ast_create_binary_op(ZEND_SL, $1.ast, $3.ast); }
	|	expr T_SR expr	{ $$.ast = zend_ast_create_binary_op(ZEND_SR, $1.ast, $3.ast); }
	|	'+' expr %prec T_INC { $$.ast = zend_ast_create_unary(ZEND_AST_UNARY_PLUS, $2.ast); }
	|	'-' expr %prec T_INC { $$.ast = zend_ast_create_unary(ZEND_AST_UNARY_MINUS, $2.ast); }
	|	'!' expr { $$.ast = zend_ast_create_ex(1, ZEND_AST_UNARY_OP, ZEND_BOOL_NOT, $2.ast); }
	|	'~' expr { $$.ast = zend_ast_create_ex(1, ZEND_AST_UNARY_OP, ZEND_BW_NOT, $2.ast); }
	|	expr T_IS_IDENTICAL expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_IDENTICAL, $1.ast, $3.ast); }
	|	expr T_IS_NOT_IDENTICAL expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_NOT_IDENTICAL, $1.ast, $3.ast); }
	|	expr T_IS_EQUAL expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_EQUAL, $1.ast, $3.ast); }
	|	expr T_IS_NOT_EQUAL expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_NOT_EQUAL, $1.ast, $3.ast); }
	|	expr '<' expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1.ast, $3.ast); }
	|	expr T_IS_SMALLER_OR_EQUAL expr
			{ $$.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $1.ast, $3.ast); }
	|	expr '>' expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_GREATER, $1.ast, $3.ast); }
	|	expr T_IS_GREATER_OR_EQUAL expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_GREATER_EQUAL, $1.ast, $3.ast); }
	|	expr T_INSTANCEOF class_name_reference
			{ $$.ast = zend_ast_create_binary(ZEND_AST_INSTANCEOF, $1.ast, $3.ast); }
	|	parenthesis_expr { $$.ast = $1.ast; }
	|	new_expr { $$.ast = $1.ast; }
	|	expr '?' expr ':' expr
			{ $$.ast = zend_ast_create_ternary(
			      ZEND_AST_CONDITIONAL, $1.ast, $3.ast, $5.ast); }
	|	expr '?' ':' expr
			{ $$.ast = zend_ast_create_ternary(ZEND_AST_CONDITIONAL, $1.ast, NULL, $4.ast); }
	|	internal_functions_in_yacc { $$.ast = $1.ast; }
	|	T_INT_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_LONG, $2.ast); }
	|	T_DOUBLE_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_DOUBLE, $2.ast); }
	|	T_STRING_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_STRING, $2.ast); }
	|	T_ARRAY_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_ARRAY, $2.ast); }
	|	T_OBJECT_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_OBJECT, $2.ast); }
	|	T_BOOL_CAST expr
			{ $$.ast = zend_ast_create_cast(_IS_BOOL, $2.ast); }
	|	T_UNSET_CAST expr
			{ $$.ast = zend_ast_create_cast(IS_NULL, $2.ast); }
	|	T_EXIT exit_expr { $$.ast = zend_ast_create_unary(ZEND_AST_EXIT, $2.ast); }
	|	'@' expr { $$.ast = zend_ast_create_unary(ZEND_AST_SILENCE, $2.ast); }
	|	scalar { $$.ast = $1.ast; }
	|	'`' backticks_expr '`' { $$.ast = zend_ast_create_unary(ZEND_AST_SHELL_EXEC, $2.ast); }
	|	T_PRINT expr { $$.ast = zend_ast_create_unary(ZEND_AST_PRINT, $2.ast); }
	|	T_YIELD { $$.ast = zend_ast_create_binary(ZEND_AST_YIELD, NULL, NULL); }
	|	T_YIELD expr { $$.ast = zend_ast_create_binary(ZEND_AST_YIELD, $2.ast, NULL); }
	|	T_YIELD expr T_DOUBLE_ARROW expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_YIELD, $4.ast, $2.ast); }
	|	function returns_ref '(' parameter_list ')' lexical_vars
			{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }
		'{' inner_statement_list '}'
			{ $$.ast = zend_ast_create_decl(ZEND_AST_CLOSURE, $2.num, $1.num, $7.str,
				  STR_INIT("{closure}", sizeof("{closure}") - 1, 0),
			      $4.ast, $6.ast, $9.ast); }
	|	T_STATIC function returns_ref '(' parameter_list ')' lexical_vars
			{ $$.str = CG(doc_comment); CG(doc_comment) = NULL; }
		'{' inner_statement_list '}'
			{ $$.ast = zend_ast_create_decl(ZEND_AST_CLOSURE, $3.num | ZEND_ACC_STATIC, $2.num,
			      $8.str, STR_INIT("{closure}", sizeof("{closure}") - 1, 0),
			      $5.ast, $7.ast, $10.ast); }
;

function:
	T_FUNCTION { $$.num = CG(zend_lineno); }
;

returns_ref:
		/* empty */	{ $$.num = 0; }
	|	'&'			{ $$.num = ZEND_ACC_RETURN_REFERENCE; }
;

lexical_vars:
		/* empty */ { $$.ast = NULL; }
	|	T_USE '(' lexical_var_list ')' { $$.ast = $3.ast; }
;

lexical_var_list:
		lexical_var_list ',' lexical_var { $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	lexical_var { $$.list = zend_ast_create_list(1, ZEND_AST_CLOSURE_USES, $1.ast); }
;

lexical_var:
		T_VARIABLE		{ $$.ast = $1.ast; }
	|	'&' T_VARIABLE	{ $$.ast = $2.ast; $$.ast->attr = 1; }
;

function_call:
		name argument_list
			{ $$.ast = zend_ast_create_binary(ZEND_AST_CALL, $1.ast, $2.ast); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      $1.ast, $3.ast, $4.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      $1.ast, $3.ast, $4.ast); }
	|	callable_expr argument_list
			{ $$.ast = zend_ast_create_binary(ZEND_AST_CALL, $1.ast, $2.ast); }
;

class_name:
		T_STATIC
			{ zval zv; ZVAL_STRINGL(&zv, "static", sizeof("static")-1);
			  $$.ast = zend_ast_create_zval_ex(&zv, ZEND_NAME_NOT_FQ); }
	|	name { $$.ast = $1.ast; }
;

class_name_reference:
		class_name		{ $$.ast = $1.ast; }
	|	new_variable	{ $$.ast = $1.ast; }
;

exit_expr:
		/* empty */			{ $$.ast = NULL; }
	|	'(' ')'				{ $$.ast = NULL; }
	|	parenthesis_expr	{ $$.ast = $1.ast; }
;

backticks_expr:
		/* empty */
			{ $$.ast = zend_ast_create_zval_from_str(STR_EMPTY_ALLOC()); }
	|	T_ENCAPSED_AND_WHITESPACE { $$.ast = $1.ast; }
	|	encaps_list { $$.ast = $1.ast; }
;


ctor_arguments:
		/* empty */	{ $$.list = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	argument_list { $$.ast = $1.ast; }
;


dereferencable_scalar:
		T_ARRAY '(' array_pair_list ')'	{ $$.ast = $3.ast; }
	|	'[' array_pair_list ']'			{ $$.ast = $2.ast; }
	|	T_CONSTANT_ENCAPSED_STRING		{ $$.ast = $1.ast; }
;

scalar:
		T_LNUMBER 	{ $$.ast = $1.ast; }
	|	T_DNUMBER 	{ $$.ast = $1.ast; }
	|	T_LINE 		{ $$.ast = $1.ast; }
	|	T_FILE 		{ $$.ast = $1.ast; }
	|	T_DIR   	{ $$.ast = $1.ast; }
	|	T_TRAIT_C	{ $$.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_TRAIT_C); }
	|	T_METHOD_C	{ $$.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_METHOD_C); }
	|	T_FUNC_C	{ $$.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_FUNC_C); }
	|	T_NS_C		{ $$.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_NS_C); }
	|	T_CLASS_C	{ $$.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_CLASS_C); }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$.ast = $2.ast; }
	|	T_START_HEREDOC T_END_HEREDOC
			{ $$.ast = zend_ast_create_zval_from_str(STR_EMPTY_ALLOC()); }
	|	'"' encaps_list '"' 	{ $$.ast = $2.ast; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$.ast = $2.ast; }
	|	dereferencable_scalar	{ $$.ast = $1.ast; }
	|	class_name_scalar	{ $$.ast = $1.ast; }
	|	class_constant		{ $$.ast = $1.ast; }
	|	name				{ $$.ast = zend_ast_create_unary(ZEND_AST_CONST, $1.ast); }
;


possible_comma:
		/* empty */
	|	','
;

expr:
		variable					{ $$.ast = $1.ast; }
	|	expr_without_variable		{ $$.ast = $1.ast; }
;

parenthesis_expr:
		'(' expr ')' { $$.ast = $2.ast; }
;

variable_class_name:
	dereferencable { $$.ast = $1.ast; }
;

dereferencable:
		variable				{ $$.ast = $1.ast; }
	|	'(' expr ')'			{ $$.ast = $2.ast; }
	|	dereferencable_scalar	{ $$.ast = $1.ast; }
;

callable_expr:
		callable_variable		{ $$.ast = $1.ast; }
	|	'(' expr ')'			{ $$.ast = $2.ast; }
	|	dereferencable_scalar	{ $$.ast = $1.ast; }
;

callable_variable:
		simple_variable
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.ast); }
	|	dereferencable '[' dim_offset ']'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.ast, $3.ast); }
	|	dereferencable '{' expr '}'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.ast, $3.ast); }
	|	dereferencable T_OBJECT_OPERATOR member_name argument_list
			{ $$.ast = zend_ast_create_ternary(ZEND_AST_METHOD_CALL, $1.ast, $3.ast, $4.ast); }
	|	function_call { $$.ast = $1.ast; }
;

variable:
		callable_variable
			{ $$.ast = $1.ast; }
	|	static_member
			{ $$.ast = $1.ast; }
	|	dereferencable T_OBJECT_OPERATOR member_name
			{ $$.ast = zend_ast_create_binary(ZEND_AST_PROP, $1.ast, $3.ast); }
;

simple_variable:
		T_VARIABLE
			{ $$.ast = $1.ast; }
	|	'$' '{' expr '}'
			{ $$.ast = $3.ast; }
	|	'$' simple_variable
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $2.ast); }
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.ast, $3.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.ast, $3.ast); }
;

new_variable:
		simple_variable
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.ast); }
	|	new_variable '[' dim_offset ']'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.ast, $3.ast); }
	|	new_variable '{' expr '}'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.ast, $3.ast); }
	|	new_variable T_OBJECT_OPERATOR member_name
			{ $$.ast = zend_ast_create_binary(ZEND_AST_PROP, $1.ast, $3.ast); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.ast, $3.ast); }
	|	new_variable T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.ast, $3.ast); }
;

dim_offset:
		/* empty */		{ $$.ast = NULL; }
	|	expr			{ $$.ast = $1.ast; }
;

member_name:
		T_STRING		{ $$.ast = $1.ast; }
	|	'{' expr '}'	{ $$.ast = $2.ast; }
	|	simple_variable	{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.ast); }
;

assignment_list:
		assignment_list ',' assignment_list_element
			{ $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	assignment_list_element
			{ $$.list = zend_ast_create_list(1, ZEND_AST_LIST, $1.ast); }
;

assignment_list_element:
		variable						{ $$.ast = $1.ast; }
	|	T_LIST '(' assignment_list ')'	{ $$.ast = $3.ast; }
	|	/* empty */						{ $$.ast = NULL; }
;


array_pair_list:
		/* empty */ { $$.list = zend_ast_create_list(0, ZEND_AST_ARRAY); }
	|	non_empty_array_pair_list possible_comma { $$.ast = $1.ast; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' array_pair
			{ $$.list = zend_ast_list_add($1.list, $3.ast); }
	|	array_pair
			{ $$.list = zend_ast_create_list(1, ZEND_AST_ARRAY, $1.ast); }
;

array_pair:
		expr T_DOUBLE_ARROW expr
			{ $$.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $3.ast, $1.ast); }
	|	expr { $$.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $1.ast, NULL); }
	|	expr T_DOUBLE_ARROW '&' variable
			{ $$.ast = zend_ast_create_ex(2, ZEND_AST_ARRAY_ELEM, 1, $4.ast, $1.ast); }
	|	'&' variable
			{ $$.ast = zend_ast_create_ex(2, ZEND_AST_ARRAY_ELEM, 1, $2.ast, NULL); }
;

encaps_list:
		encaps_list encaps_var
			{ $$.list = zend_ast_list_add($1.list, $2.ast); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE
			{ $$.list = zend_ast_list_add($1.list, $2.ast); }
	|	encaps_var
			{ $$.list = zend_ast_create_list(1, ZEND_AST_ENCAPS_LIST, $1.ast); }
	|	T_ENCAPSED_AND_WHITESPACE encaps_var
			{ $$.list = zend_ast_create_list(2, ZEND_AST_ENCAPS_LIST, $1.ast, $2.ast); }
;

encaps_var:
		T_VARIABLE
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.ast); }
	|	T_VARIABLE '[' encaps_var_offset ']'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM,
			      zend_ast_create_unary(ZEND_AST_VAR, $1.ast), $3.ast); }
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING
			{ $$.ast = zend_ast_create_binary(ZEND_AST_PROP,
			      zend_ast_create_unary(ZEND_AST_VAR, $1.ast), $3.ast); }
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $2.ast); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '}'
			{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $2.ast); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
			{ $$.ast = zend_ast_create_binary(ZEND_AST_DIM,
			      zend_ast_create_unary(ZEND_AST_VAR, $2.ast), $4.ast); }
	|	T_CURLY_OPEN variable '}' { $$.ast = $2.ast; }
;

encaps_var_offset:
		T_STRING		{ $$.ast = $1.ast; }
	|	T_NUM_STRING	{ $$.ast = $1.ast; }
	|	T_VARIABLE		{ $$.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.ast); }
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables ')' { $$.ast  = $3.ast; }
	|	T_EMPTY '(' expr ')' { $$.ast = zend_ast_create_unary(ZEND_AST_EMPTY, $3.ast); }
	|	T_INCLUDE expr
			{ $$.ast = zend_ast_create_ex(1, ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE, $2.ast); }
	|	T_INCLUDE_ONCE expr
			{ $$.ast = zend_ast_create_ex(1, ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE_ONCE, $2.ast); }
	|	T_EVAL '(' expr ')'
			{ $$.ast = zend_ast_create_ex(1, ZEND_AST_INCLUDE_OR_EVAL, ZEND_EVAL, $3.ast); }
	|	T_REQUIRE expr
			{ $$.ast = zend_ast_create_ex(1, ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE, $2.ast); }
	|	T_REQUIRE_ONCE expr
			{ $$.ast = zend_ast_create_ex(1, ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE_ONCE, $2.ast); }
;

isset_variables:
		isset_variable { $$.ast = $1.ast; }
	|	isset_variables ',' isset_variable
			{ $$.ast = zend_ast_create_binary(ZEND_AST_AND, $1.ast, $3.ast); }
;

isset_variable:
		expr { $$.ast = zend_ast_create_unary(ZEND_AST_ISSET, $1.ast); }
;

class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$.ast = zend_ast_create_binary(
			      ZEND_AST_CLASS_CONST, $1.ast, $3.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$.ast = zend_ast_create_binary(
			      ZEND_AST_CLASS_CONST, $1.ast, $3.ast); }
;

class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS
		{ $$.ast = zend_ast_create_unary(ZEND_AST_RESOLVE_CLASS_NAME, $1.ast); }
;

%%

/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T zend_yytnamerr(char *yyres, const char *yystr)
{
	if (!yyres) {
		return yystrlen(yystr);
	}
	{
		TSRMLS_FETCH();
		if (CG(parse_error) == 0) {
			char buffer[120];
			const unsigned char *end, *str, *tok1 = NULL, *tok2 = NULL;
			unsigned int len = 0, toklen = 0, yystr_len;
			
			CG(parse_error) = 1;

			if (LANG_SCNG(yy_text)[0] == 0 &&
				LANG_SCNG(yy_leng) == 1 &&
				memcmp(yystr, "\"end of file\"", sizeof("\"end of file\"") - 1) == 0) {
				yystpcpy(yyres, "end of file");
				return sizeof("end of file")-1;
			}
			
			str = LANG_SCNG(yy_text);
			end = memchr(str, '\n', LANG_SCNG(yy_leng));
			yystr_len = yystrlen(yystr);
			
			if ((tok1 = memchr(yystr, '(', yystr_len)) != NULL
				&& (tok2 = zend_memrchr(yystr, ')', yystr_len)) != NULL) {
				toklen = (tok2 - tok1) + 1;
			} else {
				tok1 = tok2 = NULL;
				toklen = 0;
			}
			
			if (end == NULL) {
				len = LANG_SCNG(yy_leng) > 30 ? 30 : LANG_SCNG(yy_leng);
			} else {
				len = (end - str) > 30 ? 30 : (end - str);
			}
			if (toklen) {
				snprintf(buffer, sizeof(buffer), "'%.*s' %.*s", len, str, toklen, tok1);
			} else {
				snprintf(buffer, sizeof(buffer), "'%.*s'", len, str);
			}
			yystpcpy(yyres, buffer);
			return len + (toklen ? toklen + 1 : 0) + 2;
		}		
	}	
	if (*yystr == '"') {
		YYSIZE_T yyn = 0;
		const char *yyp = yystr;

		for (; *++yyp != '"'; ++yyn) {
			yyres[yyn] = *yyp;
		}
		yyres[yyn] = '\0';
		return yyn;
	}
	yystpcpy(yyres, yystr);
	return strlen(yystr);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
