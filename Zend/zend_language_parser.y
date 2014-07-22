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
#define YYSTYPE znode

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
	top_statement_list	{ zend_compile_top_stmt($1.u.ast TSRMLS_CC); zend_ast_destroy($1.u.ast); zend_do_end_compilation(TSRMLS_C); }
;

top_statement_list:
		top_statement_list top_statement { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $2.u.ast); }
	|	/* empty */ { $$.u.ast = zend_ast_create_dynamic(ZEND_AST_STMT_LIST); }
;

namespace_name:
		T_STRING { $$ = $1; }
	|	namespace_name T_NS_SEPARATOR T_STRING { zend_do_build_namespace_name(&$$, &$1, &$3 TSRMLS_CC); }
;

name:
		namespace_name
			{ $$.u.ast = zend_ast_create_zval_ex(&$1.u.constant, ZEND_NAME_NOT_FQ); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name
			{ $$.u.ast = zend_ast_create_zval_ex(&$3.u.constant, ZEND_NAME_RELATIVE); }
	|	T_NS_SEPARATOR namespace_name
			{ $$.u.ast = zend_ast_create_zval_ex(&$2.u.constant, ZEND_NAME_FQ); }
;

top_statement:
		statement						{ $$.u.ast = $1.u.ast; }
	|	function_declaration_statement	{ $$.u.ast = $1.u.ast; }
	|	class_declaration_statement		{ $$.u.ast = $1.u.ast; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ zval offset_zv; ZVAL_LONG(&offset_zv, zend_get_scanned_file_offset(TSRMLS_C));
			  $$.u.ast = zend_ast_create_unary(ZEND_AST_HALT_COMPILER,
			      zend_ast_create_zval(&offset_zv));
			  zend_stop_lexing(TSRMLS_C); }
	|	T_NAMESPACE namespace_name ';'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, AST_ZVAL(&$2), NULL);
			  zend_discard_doc_comment(TSRMLS_C); }
	|	T_NAMESPACE namespace_name { zend_discard_doc_comment(TSRMLS_C); }
		'{' top_statement_list '}'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, AST_ZVAL(&$2), $5.u.ast); }
	|	T_NAMESPACE { zend_discard_doc_comment(TSRMLS_C); }
		'{' top_statement_list '}'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_NAMESPACE, NULL, $4.u.ast); }
	|	T_USE use_declarations ';'
			{ $$.u.ast = $2.u.ast; $$.u.ast->attr = T_CLASS; }
	|	T_USE T_FUNCTION use_declarations ';'
			{ $$.u.ast = $3.u.ast; $$.u.ast->attr = T_FUNCTION; }
	|	T_USE T_CONST use_declarations ';'
			{ $$.u.ast = $3.u.ast; $$.u.ast->attr = T_CONST; }
	|	T_CONST const_list ';'			{ $$.u.ast = $2.u.ast; }
;

use_declarations:
		use_declarations ',' use_declaration
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	use_declaration
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_USE, $1.u.ast); }
;

use_declaration:
		namespace_name
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, AST_ZVAL(&$1), NULL); }
	|	namespace_name T_AS T_STRING
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, AST_ZVAL(&$1), AST_ZVAL(&$3)); }
	|	T_NS_SEPARATOR namespace_name
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, AST_ZVAL(&$2), NULL); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_USE_ELEM, AST_ZVAL(&$2), AST_ZVAL(&$4)); }
;

const_list:
		const_list ',' const_decl { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	const_decl
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_CONST_DECL, $1.u.ast); }
;

inner_statement_list:
		inner_statement_list inner_statement
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $2.u.ast); }
	|	/* empty */
			{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_STMT_LIST); }
;


inner_statement:
		statement { $$.u.ast = $1.u.ast; }
	|	function_declaration_statement { $$.u.ast = $1.u.ast; }
	|	class_declaration_statement { $$.u.ast = $1.u.ast; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ zend_error_noreturn(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
;


statement:
		unticked_statement { $$.u.ast = $1.u.ast; }
	|	T_STRING ':' { $$.u.ast = zend_ast_create_unary(ZEND_AST_LABEL, AST_ZVAL(&$1)); }
;

unticked_statement:
		'{' inner_statement_list '}' { $$.u.ast = $2.u.ast; }
	|	if_stmt { $$.u.ast = $1.u.ast; }
	|	alt_if_stmt { $$.u.ast = $1.u.ast; }
	|	T_WHILE parenthesis_expr while_statement
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_WHILE, $2.u.ast, $3.u.ast); }
	|	T_DO statement T_WHILE parenthesis_expr ';'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DO_WHILE, $2.u.ast, $4.u.ast); }
	|	T_FOR '(' for_expr ';' for_expr ';' for_expr ')' for_statement
			{ $$.u.ast = zend_ast_create(4, ZEND_AST_FOR, $3.u.ast, $5.u.ast, $7.u.ast, $9.u.ast); }
	|	T_SWITCH parenthesis_expr switch_case_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_SWITCH, $2.u.ast, $3.u.ast); }
	|	T_BREAK ';'			{ $$.u.ast = zend_ast_create_unary(ZEND_BRK, NULL); }
	|	T_BREAK expr ';'	{ $$.u.ast = zend_ast_create_unary(ZEND_BRK, $2.u.ast); }
	|	T_CONTINUE ';'		{ $$.u.ast = zend_ast_create_unary(ZEND_CONT, NULL); }
	|	T_CONTINUE expr ';'	{ $$.u.ast = zend_ast_create_unary(ZEND_CONT, $2.u.ast); }
	|	T_RETURN ';'
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_RETURN, NULL); }
	|	T_RETURN expr ';'
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_RETURN, $2.u.ast); }
	|	T_GLOBAL global_var_list ';' { $$.u.ast = $2.u.ast; }
	|	T_STATIC static_var_list ';' { $$.u.ast = $2.u.ast; }
	|	T_ECHO echo_expr_list ';' { $$.u.ast = $2.u.ast; }
	|	T_INLINE_HTML { $$.u.ast = zend_ast_create_unary(ZEND_ECHO, AST_ZVAL(&$1)); }
	|	expr ';' { $$.u.ast = $1.u.ast; }
	|	T_UNSET '(' unset_variables ')' ';' { $$.u.ast = $3.u.ast; }
	|	T_FOREACH '(' expr T_AS foreach_variable ')' foreach_statement
			{ $$.u.ast = zend_ast_create(4, ZEND_AST_FOREACH,
			      $3.u.ast, $5.u.ast, NULL, $7.u.ast); }
	|	T_FOREACH '(' expr T_AS foreach_variable T_DOUBLE_ARROW foreach_variable ')'
		foreach_statement
			{ $$.u.ast = zend_ast_create(4, ZEND_AST_FOREACH,
			      $3.u.ast, $7.u.ast, $5.u.ast, $9.u.ast); }
	|	T_DECLARE '(' const_list ')' declare_statement
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DECLARE, $3.u.ast, $5.u.ast); }
	|	';'	/* empty statement */ { $$.u.ast = NULL; }
	|	T_TRY '{' inner_statement_list '}' catch_list finally_statement
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_TRY, $3.u.ast, $5.u.ast, $6.u.ast); }
	|	T_THROW expr ';' { $$.u.ast = zend_ast_create_unary(ZEND_THROW, $2.u.ast); }
	|	T_GOTO T_STRING ';' { $$.u.ast = zend_ast_create_unary(ZEND_GOTO, AST_ZVAL(&$2)); }
;

catch_list:
		/* empty */
			{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_CATCH_LIST); }
	|	catch_list T_CATCH '(' name T_VARIABLE ')' '{' inner_statement_list '}'
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_ternary(ZEND_AST_CATCH, $4.u.ast, AST_ZVAL(&$5), $8.u.ast)); }
;

finally_statement:
		/* empty */ { $$.u.ast = NULL; }
	|	T_FINALLY '{' inner_statement_list '}' { $$.u.ast = $3.u.ast; }
;

unset_variables:
		unset_variable { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_STMT_LIST, $1.u.ast); }
	|	unset_variables ',' unset_variable { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
;

unset_variable:
		variable { $$.u.ast = zend_ast_create_unary(ZEND_AST_UNSET, $1.u.ast); }
;

function_declaration_statement:
	function returns_ref T_STRING '(' parameter_list ')' '{' inner_statement_list '}'
		{ $$.u.ast = zend_ast_create_decl(ZEND_AST_FUNC_DECL, $2.EA,
		      $1.EA, CG(zend_lineno), LANG_SCNG(yy_text), $1.u.op.ptr,
			  Z_STR($3.u.constant), $5.u.ast, NULL, $8.u.ast); }
;

is_reference:
		/* empty */	{ $$.op_type = 0; }
	|	'&'			{ $$.op_type = ZEND_PARAM_REF; }
;

is_variadic:
		/* empty */ { $$.op_type = 0; }
	|	T_ELLIPSIS  { $$.op_type = ZEND_PARAM_VARIADIC; }
;

class_declaration_statement:
		class_entry_type T_STRING extends_from implements_list
			{ $$.u.op.ptr = CG(doc_comment); CG(doc_comment) = NULL; }	
		'{' class_statement_list '}'
			{ $$.u.ast = zend_ast_create_decl(ZEND_AST_CLASS, $1.EA, $1.u.op.opline_num,
			      CG(zend_lineno), LANG_SCNG(yy_text), $5.u.op.ptr,
				  Z_STR($2.u.constant), $3.u.ast, $4.u.ast, $7.u.ast); }
	|	interface_entry T_STRING interface_extends_list
			{ $$.u.op.ptr = CG(doc_comment); CG(doc_comment) = NULL; }	
		'{' class_statement_list '}'
			{ $$.u.ast = zend_ast_create_decl(ZEND_AST_CLASS, $1.EA, $1.u.op.opline_num,
			      CG(zend_lineno), LANG_SCNG(yy_text), $4.u.op.ptr,
				  Z_STR($2.u.constant), NULL, $3.u.ast, $6.u.ast); }
;


class_entry_type:
		T_CLASS			{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = 0; }
	|	T_ABSTRACT T_CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
	|	T_TRAIT { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_TRAIT; }
	|	T_FINAL T_CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_FINAL_CLASS; }
;

interface_entry:
	T_INTERFACE		{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_INTERFACE; }
;

extends_from:
		/* empty */		{ $$.u.ast = NULL; }
	|	T_EXTENDS name	{ $$.u.ast = $2.u.ast; }
;

interface_extends_list:
		/* empty */				{ $$.u.ast = NULL; }
	|	T_EXTENDS name_list	{ $$.u.ast = $2.u.ast; }
;

implements_list:
		/* empty */				{ $$.u.ast = NULL; }
	|	T_IMPLEMENTS name_list	{ $$.u.ast = $2.u.ast; }
;

foreach_variable:
		variable			{ $$.u.ast = $1.u.ast; }
	|	'&' variable		{ $$.u.ast = zend_ast_create_unary(ZEND_AST_REF, $2.u.ast); }
	|	T_LIST '(' assignment_list ')' { $$.u.ast = $3.u.ast; }
;

for_statement:
		statement { $$.u.ast = $1.u.ast; }
	|	':' inner_statement_list T_ENDFOR ';' { $$.u.ast = $2.u.ast; }
;


foreach_statement:
		statement { $$.u.ast = $1.u.ast; }
	|	':' inner_statement_list T_ENDFOREACH ';' { $$.u.ast = $2.u.ast; }
;


declare_statement:
		statement { $$.u.ast = $1.u.ast; }
	|	':' inner_statement_list T_ENDDECLARE ';' { $$.u.ast = $2.u.ast; }
;

switch_case_list:
		'{' case_list '}'					{ $$.u.ast = $2.u.ast; }
	|	'{' ';' case_list '}'				{ $$.u.ast = $3.u.ast; }
	|	':' case_list T_ENDSWITCH ';'		{ $$.u.ast = $2.u.ast; }
	|	':' ';' case_list T_ENDSWITCH ';'	{ $$.u.ast = $3.u.ast; }
;

case_list:
		/* empty */ { $$.u.ast = zend_ast_create_dynamic(ZEND_AST_SWITCH_LIST); }
	|	case_list T_CASE expr case_separator inner_statement_list
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_SWITCH_CASE, $3.u.ast, $5.u.ast)); }
	|	case_list T_DEFAULT case_separator inner_statement_list
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_SWITCH_CASE, NULL, $4.u.ast)); }
;

case_separator:
		':'
	|	';'
;


while_statement:
		statement { $$.u.ast = $1.u.ast; }
	|	':' inner_statement_list T_ENDWHILE ';' { $$.u.ast = $2.u.ast; }
;


if_stmt_without_else:
		T_IF parenthesis_expr statement
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_IF,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $2.u.ast, $3.u.ast)); }
	|	if_stmt_without_else T_ELSEIF parenthesis_expr statement
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $3.u.ast, $4.u.ast)); }
;

if_stmt:
		if_stmt_without_else { $$.u.ast = $1.u.ast; }
	|	if_stmt_without_else T_ELSE statement
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, NULL, $3.u.ast)); }
;

alt_if_stmt_without_else:
		T_IF parenthesis_expr ':' inner_statement_list
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_IF,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $2.u.ast, $4.u.ast)); }
	|	alt_if_stmt_without_else T_ELSEIF parenthesis_expr ':' inner_statement_list
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, $3.u.ast, $5.u.ast)); }
;

alt_if_stmt:
		alt_if_stmt_without_else T_ENDIF ';' { $$.u.ast = $1.u.ast; }
	|	alt_if_stmt_without_else T_ELSE ':' inner_statement_list T_ENDIF ';'
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast,
			      zend_ast_create_binary(ZEND_AST_IF_ELEM, NULL, $4.u.ast)); }
;

parameter_list:
		non_empty_parameter_list { $$.u.ast = $1.u.ast; }
	|	/* empty */	{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_PARAM_LIST); }
;


non_empty_parameter_list:
		parameter
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_PARAM_LIST, $1.u.ast); }
	|	non_empty_parameter_list ',' parameter
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
;

parameter:
		optional_type is_reference is_variadic T_VARIABLE
			{ $$.u.ast = zend_ast_create_ex(3, ZEND_AST_PARAM, $2.op_type | $3.op_type,
			      $1.u.ast, AST_ZVAL(&$4), NULL); }
	|	optional_type is_reference is_variadic T_VARIABLE '=' expr
			{ $$.u.ast = zend_ast_create_ex(3, ZEND_AST_PARAM, $2.op_type | $3.op_type,
			      $1.u.ast, AST_ZVAL(&$4), $6.u.ast); }
;


optional_type:
		/* empty */	{ $$.u.ast = NULL; }
	|	T_ARRAY		{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_TYPE, IS_ARRAY); }
	|	T_CALLABLE	{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_TYPE, IS_CALLABLE); }
	|	name		{ $$.u.ast = $1.u.ast; }
;

argument_list:
		'(' ')'	{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_ARG_LIST); }
	|	'(' non_empty_argument_list ')' { $$.u.ast = $2.u.ast; }
;

non_empty_argument_list:
		argument
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_ARG_LIST, $1.u.ast); }
	|	non_empty_argument_list ',' argument
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
;

argument:
		expr_without_variable	{ $$.u.ast = $1.u.ast; }
	|	variable				{ $$.u.ast = $1.u.ast; }
	/*|	'&' variable 			{ ZEND_ASSERT(0); } */
	|	T_ELLIPSIS expr			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_UNPACK, $2.u.ast); }
;

global_var_list:
		global_var_list ',' global_var { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	global_var { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_STMT_LIST, $1.u.ast); }
;

global_var:
	simple_variable
		{ $$.u.ast = zend_ast_create_unary(ZEND_AST_GLOBAL,
		      zend_ast_create_unary(ZEND_AST_VAR, $1.u.ast)); }
;


static_var_list:
		static_var_list ',' static_var { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	static_var { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_STMT_LIST, $1.u.ast); }
;

static_var:
		T_VARIABLE
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC, AST_ZVAL(&$1), NULL); }
	|	T_VARIABLE '=' expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC, AST_ZVAL(&$1), $3.u.ast); }
;


class_statement_list:
		class_statement_list class_statement
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $2.u.ast); }
	|	/* empty */
			{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_STMT_LIST); }
;


class_statement:
		variable_modifiers property_list ';'
			{ $$.u.ast = $2.u.ast; $$.u.ast->attr = Z_LVAL($1.u.constant); }
	|	T_CONST class_const_list ';'
			{ $$.u.ast = $2.u.ast; zend_discard_doc_comment(TSRMLS_C); }
	|	T_USE name_list trait_adaptations
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_USE_TRAIT, $2.u.ast, $3.u.ast); }
	|	method_modifiers function returns_ref T_STRING '(' parameter_list ')' method_body
			{ $$.u.ast = zend_ast_create_decl(ZEND_AST_METHOD, $3.EA | Z_LVAL($1.u.constant),
			      $2.EA, CG(zend_lineno), LANG_SCNG(yy_text), $2.u.op.ptr,
				  Z_STR($4.u.constant), $6.u.ast, NULL, $8.u.ast); }
;

name_list:
		name { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_NAME_LIST, $1.u.ast); }
	|	name_list ',' name { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
;

trait_adaptations:
		';'								{ $$.u.ast = NULL; }
	|	'{' '}'							{ $$.u.ast = NULL; }
	|	'{' trait_adaptation_list '}'	{ $$.u.ast = $2.u.ast; }
;

trait_adaptation_list:
		trait_adaptation
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_TRAIT_ADAPTATIONS, $1.u.ast); }
	|	trait_adaptation_list trait_adaptation
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $2.u.ast); }
;

trait_adaptation:
		trait_precedence ';'	{ $$.u.ast = $1.u.ast; }
	|	trait_alias ';'			{ $$.u.ast = $1.u.ast; }
;

trait_precedence:
	absolute_trait_method_reference T_INSTEADOF name_list
		{ $$.u.ast = zend_ast_create_binary(ZEND_AST_TRAIT_PRECEDENCE, $1.u.ast, $3.u.ast); }
;

trait_alias:
		trait_method_reference T_AS trait_modifiers T_STRING
			{ $$.u.ast = zend_ast_create_ex(2, ZEND_AST_TRAIT_ALIAS,
			      Z_LVAL($3.u.constant), $1.u.ast, AST_ZVAL(&$4)); }
	|	trait_method_reference T_AS member_modifier
			{ $$.u.ast = zend_ast_create_ex(2, ZEND_AST_TRAIT_ALIAS,
			      Z_LVAL($3.u.constant), $1.u.ast, NULL); }
;

trait_method_reference:
		T_STRING
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_METHOD_REFERENCE, NULL, AST_ZVAL(&$1)); }
	|	absolute_trait_method_reference { $$.u.ast = $1.u.ast; }
;

absolute_trait_method_reference:
	name T_PAAMAYIM_NEKUDOTAYIM T_STRING
		{ $$.u.ast = zend_ast_create_binary(ZEND_AST_METHOD_REFERENCE, $1.u.ast, AST_ZVAL(&$3)); }
;

trait_modifiers:
		/* empty */		{ Z_LVAL($$.u.constant) = 0; }
	|	member_modifier	{ $$ = $1; }
;

method_body:
		';' /* abstract method */		{ $$.u.ast = NULL; }
	|	'{' inner_statement_list '}'	{ $$.u.ast = $2.u.ast; }
;

variable_modifiers:
		non_empty_member_modifiers		{ $$ = $1; }
	|	T_VAR							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
;

method_modifiers:
		/* empty */						{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers		{ $$ = $1; if (!(Z_LVAL($$.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL($$.u.constant) |= ZEND_ACC_PUBLIC; } }
;

non_empty_member_modifiers:
		member_modifier						{ $$ = $1; }
	|	non_empty_member_modifiers member_modifier	{ Z_LVAL($$.u.constant) = zend_do_verify_access_types(&$1, &$2); }
;

member_modifier:
		T_PUBLIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
	|	T_PROTECTED				{ Z_LVAL($$.u.constant) = ZEND_ACC_PROTECTED; }
	|	T_PRIVATE				{ Z_LVAL($$.u.constant) = ZEND_ACC_PRIVATE; }
	|	T_STATIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_STATIC; }
	|	T_ABSTRACT				{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
	|	T_FINAL					{ Z_LVAL($$.u.constant) = ZEND_ACC_FINAL; }
;

property_list:
		property_list ',' property { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	property { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_PROP_DECL, $1.u.ast); }
;

property:
		T_VARIABLE { $$.u.ast = zend_ast_create_binary(ZEND_AST_PROP_ELEM, AST_ZVAL(&$1), NULL); }
	|	T_VARIABLE '=' expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_PROP_ELEM, AST_ZVAL(&$1), $3.u.ast); }
;

class_const_list:
		class_const_list ',' const_decl { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	const_decl
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_CLASS_CONST_DECL, $1.u.ast); }
;

const_decl:
	T_STRING '=' expr
		{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CONST_ELEM, AST_ZVAL(&$1), $3.u.ast); }
;

echo_expr_list:
		echo_expr_list ',' echo_expr { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	echo_expr { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_STMT_LIST, $1.u.ast); }
;
echo_expr:
	expr { $$.u.ast = zend_ast_create_unary(ZEND_ECHO, $1.u.ast); }
;

for_expr:
		/* empty */			{ $$.u.ast = NULL; }
	|	non_empty_for_expr	{ $$.u.ast = $1.u.ast; }
;

non_empty_for_expr:
		non_empty_for_expr ',' expr { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	expr { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_EXPR_LIST, $1.u.ast); }
;

new_expr:
		T_NEW class_name_reference ctor_arguments
			{ $$.u.ast = zend_ast_create_binary(ZEND_NEW, $2.u.ast, $3.u.ast); }
;

expr_without_variable:
		T_LIST '(' assignment_list ')' '=' expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ASSIGN, $3.u.ast, $6.u.ast); }
	|	variable '=' expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ASSIGN, $1.u.ast, $3.u.ast); }
	|	variable '=' '&' variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ASSIGN_REF, $1.u.ast, $4.u.ast); }
	|	variable '=' '&' T_NEW class_name_reference ctor_arguments
			{ zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");
			  $$.u.ast = zend_ast_create_binary(ZEND_AST_ASSIGN_REF, $1.u.ast,
			      zend_ast_create_binary(ZEND_NEW, $5.u.ast, $6.u.ast)); }
	|	T_CLONE expr { $$.u.ast = zend_ast_create_unary(ZEND_CLONE, $2.u.ast); }
	|	variable T_PLUS_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_ADD, $1.u.ast, $3.u.ast); }
	|	variable T_MINUS_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SUB, $1.u.ast, $3.u.ast); }
	|	variable T_MUL_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_MUL, $1.u.ast, $3.u.ast); }
	|	variable T_POW_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_POW, $1.u.ast, $3.u.ast); }
	|	variable T_DIV_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_DIV, $1.u.ast, $3.u.ast); }
	|	variable T_CONCAT_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_CONCAT, $1.u.ast, $3.u.ast); }
	|	variable T_MOD_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_MOD, $1.u.ast, $3.u.ast); }
	|	variable T_AND_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_AND, $1.u.ast, $3.u.ast); }
	|	variable T_OR_EQUAL expr 
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_OR, $1.u.ast, $3.u.ast); }
	|	variable T_XOR_EQUAL expr 
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_BW_XOR, $1.u.ast, $3.u.ast); }
	|	variable T_SL_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SL, $1.u.ast, $3.u.ast); }
	|	variable T_SR_EQUAL expr
			{ $$.u.ast = zend_ast_create_assign_op(ZEND_ASSIGN_SR, $1.u.ast, $3.u.ast); }
	|	variable T_INC { $$.u.ast = zend_ast_create_unary(ZEND_POST_INC, $1.u.ast); }
	|	T_INC variable { $$.u.ast = zend_ast_create_unary(ZEND_PRE_INC, $2.u.ast); }
	|	variable T_DEC { $$.u.ast = zend_ast_create_unary(ZEND_POST_DEC, $1.u.ast); }
	|	T_DEC variable { $$.u.ast = zend_ast_create_unary(ZEND_PRE_DEC, $2.u.ast); }
	|	expr T_BOOLEAN_OR expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_OR, $1.u.ast, $3.u.ast); }
	|	expr T_BOOLEAN_AND expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_AND, $1.u.ast, $3.u.ast); }
	|	expr T_LOGICAL_OR expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_OR, $1.u.ast, $3.u.ast); }
	|	expr T_LOGICAL_AND expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_AND, $1.u.ast, $3.u.ast); }
	|	expr T_LOGICAL_XOR expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_BOOL_XOR, $1.u.ast, $3.u.ast); }
	|	expr '|' expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_OR, $1.u.ast, $3.u.ast); }
	|	expr '&' expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_AND, $1.u.ast, $3.u.ast); }
	|	expr '^' expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_XOR, $1.u.ast, $3.u.ast); }
	|	expr '.' expr 	{ $$.u.ast = zend_ast_create_binary_op(ZEND_CONCAT, $1.u.ast, $3.u.ast); }
	|	expr '+' expr 	{ $$.u.ast = zend_ast_create_binary_op(ZEND_ADD, $1.u.ast, $3.u.ast); }
	|	expr '-' expr 	{ $$.u.ast = zend_ast_create_binary_op(ZEND_SUB, $1.u.ast, $3.u.ast); }
	|	expr '*' expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_MUL, $1.u.ast, $3.u.ast); }
	|	expr T_POW expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_POW, $1.u.ast, $3.u.ast); }
	|	expr '/' expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_DIV, $1.u.ast, $3.u.ast); }
	|	expr '%' expr 	{ $$.u.ast = zend_ast_create_binary_op(ZEND_MOD, $1.u.ast, $3.u.ast); }
	| 	expr T_SL expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_SL, $1.u.ast, $3.u.ast); }
	|	expr T_SR expr	{ $$.u.ast = zend_ast_create_binary_op(ZEND_SR, $1.u.ast, $3.u.ast); }
	|	'+' expr %prec T_INC { $$.u.ast = zend_ast_create_unary(ZEND_AST_UNARY_PLUS, $2.u.ast); }
	|	'-' expr %prec T_INC { $$.u.ast = zend_ast_create_unary(ZEND_AST_UNARY_MINUS, $2.u.ast); }
	|	'!' expr { $$.u.ast = zend_ast_create_unary(ZEND_BOOL_NOT, $2.u.ast); }
	|	'~' expr { $$.u.ast = zend_ast_create_unary(ZEND_BW_NOT, $2.u.ast); }
	|	expr T_IS_IDENTICAL expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	expr T_IS_NOT_IDENTICAL expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_NOT_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	expr T_IS_EQUAL expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_EQUAL, $1.u.ast, $3.u.ast); }
	|	expr T_IS_NOT_EQUAL expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_NOT_EQUAL, $1.u.ast, $3.u.ast); }
	|	expr '<' expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1.u.ast, $3.u.ast); }
	|	expr T_IS_SMALLER_OR_EQUAL expr
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $1.u.ast, $3.u.ast); }
	|	expr '>' expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_GREATER, $1.u.ast, $3.u.ast); }
	|	expr T_IS_GREATER_OR_EQUAL expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_GREATER_EQUAL, $1.u.ast, $3.u.ast); }
	|	expr T_INSTANCEOF class_name_reference
			{ $$.u.ast = zend_ast_create_binary(ZEND_INSTANCEOF, $1.u.ast, $3.u.ast); }
	|	parenthesis_expr { $$.u.ast = $1.u.ast; }
	|	new_expr { $$.u.ast = $1.u.ast; }
	|	expr '?' expr ':' expr
			{ $$.u.ast = zend_ast_create_ternary(
			      ZEND_AST_CONDITIONAL, $1.u.ast, $3.u.ast, $5.u.ast); }
	|	expr '?' ':' expr
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_CONDITIONAL, $1.u.ast, NULL, $4.u.ast); }
	|	internal_functions_in_yacc { $$.u.ast = $1.u.ast; }
	|	T_INT_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_LONG, $2.u.ast); }
	|	T_DOUBLE_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_DOUBLE, $2.u.ast); }
	|	T_STRING_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_STRING, $2.u.ast); }
	|	T_ARRAY_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_ARRAY, $2.u.ast); }
	|	T_OBJECT_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_OBJECT, $2.u.ast); }
	|	T_BOOL_CAST expr
			{ $$.u.ast = zend_ast_create_cast(_IS_BOOL, $2.u.ast); }
	|	T_UNSET_CAST expr
			{ $$.u.ast = zend_ast_create_cast(IS_NULL, $2.u.ast); }
	|	T_EXIT exit_expr { $$.u.ast = zend_ast_create_unary(ZEND_EXIT, $2.u.ast); }
	|	'@' expr { $$.u.ast = zend_ast_create_unary(ZEND_AST_SILENCE, $2.u.ast); }
	|	scalar { $$.u.ast = $1.u.ast; }
	|	'`' backticks_expr '`' { $$.u.ast = zend_ast_create_unary(ZEND_AST_SHELL_EXEC, $2.u.ast); }
	|	T_PRINT expr { $$.u.ast = zend_ast_create_unary(ZEND_PRINT, $2.u.ast); }
	|	T_YIELD { $$.u.ast = zend_ast_create_binary(ZEND_YIELD, NULL, NULL); }
	|	T_YIELD expr { $$.u.ast = zend_ast_create_binary(ZEND_YIELD, $2.u.ast, NULL); }
	|	T_YIELD expr T_DOUBLE_ARROW expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_YIELD, $4.u.ast, $2.u.ast); }
	|	function returns_ref '(' parameter_list ')' lexical_vars '{' inner_statement_list '}'
			{ $$.u.ast = zend_ast_create_decl(ZEND_AST_CLOSURE, $2.EA,
			      $1.EA, CG(zend_lineno), LANG_SCNG(yy_text), $1.u.op.ptr,
				  STR_INIT("{closure}", sizeof("{closure}") - 1, 0),
			      $4.u.ast, $6.u.ast, $8.u.ast); }
	|	T_STATIC function returns_ref '(' parameter_list ')' lexical_vars
		'{' inner_statement_list '}'
			{ $$.u.ast = zend_ast_create_decl(ZEND_AST_CLOSURE,
			      $3.EA | ZEND_ACC_STATIC, $2.EA, CG(zend_lineno), LANG_SCNG(yy_text),
			      $2.u.op.ptr, STR_INIT("{closure}", sizeof("{closure}") - 1, 0),
			      $5.u.ast, $7.u.ast, $9.u.ast); }
;

function:
	T_FUNCTION
		{ $$.EA = CG(zend_lineno); $$.u.op.ptr = CG(doc_comment); CG(doc_comment) = NULL; }
;

returns_ref:
		/* empty */	{ $$.EA = 0; }
	|	'&'			{ $$.EA = ZEND_ACC_RETURN_REFERENCE; }
;

lexical_vars:
		/* empty */ { $$.u.ast = NULL; }
	|	T_USE '(' lexical_var_list ')' { $$.u.ast = $3.u.ast; }
;

lexical_var_list:
		lexical_var_list ',' lexical_var { $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	lexical_var { $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_CLOSURE_USES, $1.u.ast); }
;

lexical_var:
		T_VARIABLE		{ $$.u.ast = AST_ZVAL(&$1); }
	|	'&' T_VARIABLE	{ $$.u.ast = zend_ast_create_zval_ex(&$2.u.constant, 1); }
;

function_call:
		name argument_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL, $1.u.ast, $2.u.ast); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      $1.u.ast, $3.u.ast, $4.u.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      $1.u.ast, $3.u.ast, $4.u.ast); }
	|	callable_expr argument_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL, $1.u.ast, $2.u.ast); }
;

class_name:
		T_STATIC
			{ zval zv; ZVAL_STRINGL(&zv, "static", sizeof("static")-1);
			  $$.u.ast = zend_ast_create_zval_ex(&zv, ZEND_NAME_NOT_FQ); }
	|	name { $$.u.ast = $1.u.ast; }
;

class_name_reference:
		class_name		{ $$.u.ast = $1.u.ast; }
	|	new_variable	{ $$.u.ast = $1.u.ast; }
;

exit_expr:
		/* empty */			{ $$.u.ast = NULL; }
	|	'(' ')'				{ $$.u.ast = NULL; }
	|	parenthesis_expr	{ $$.u.ast = $1.u.ast; }
;

backticks_expr:
		/* empty */
			{ zval empty_str; ZVAL_EMPTY_STRING(&empty_str);
			  $$.u.ast = zend_ast_create_zval(&empty_str); }
	|	T_ENCAPSED_AND_WHITESPACE { $$.u.ast = AST_ZVAL(&$1); }
	|	encaps_list { $$.u.ast = $1.u.ast; }
;


ctor_arguments:
		/* empty */	{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_ARG_LIST); }
	|	argument_list { $$.u.ast = $1.u.ast; }
;


dereferencable_scalar:
		T_ARRAY '(' array_pair_list ')'	{ $$.u.ast = $3.u.ast; }
	|	'[' array_pair_list ']'			{ $$.u.ast = $2.u.ast; }
	|	T_CONSTANT_ENCAPSED_STRING		{ $$.u.ast = AST_ZVAL(&$1); }
;

scalar:
		T_LNUMBER 	{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_DNUMBER 	{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_LINE 		{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_FILE 		{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_DIR   	{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_TRAIT_C	{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_TRAIT_C); }
	|	T_METHOD_C	{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_METHOD_C); }
	|	T_FUNC_C	{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_FUNC_C); }
	|	T_NS_C		{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_NS_C); }
	|	T_CLASS_C	{ $$.u.ast = zend_ast_create_ex(0, ZEND_AST_MAGIC_CONST, T_CLASS_C); }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$.u.ast = AST_ZVAL(&$2); }
	|	T_START_HEREDOC T_END_HEREDOC
			{ zval empty_str; ZVAL_EMPTY_STRING(&empty_str);
			  $$.u.ast = zend_ast_create_zval(&empty_str); }
	|	'"' encaps_list '"' 	{ $$.u.ast = $2.u.ast; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$.u.ast = $2.u.ast; }
	|	dereferencable_scalar	{ $$.u.ast = $1.u.ast; }
	|	class_name_scalar	{ $$.u.ast = $1.u.ast; }
	|	class_constant		{ $$.u.ast = $1.u.ast; }
	|	name				{ $$.u.ast = zend_ast_create_unary(ZEND_AST_CONST, $1.u.ast); }
;


possible_comma:
		/* empty */
	|	','
;

expr:
		variable					{ $$.u.ast = $1.u.ast; }
	|	expr_without_variable		{ $$.u.ast = $1.u.ast; }
;

parenthesis_expr:
		'(' expr ')' { $$.u.ast = $2.u.ast; }
;

variable_class_name:
	dereferencable { $$.u.ast = $1.u.ast; }
;

dereferencable:
		variable				{ $$.u.ast = $1.u.ast; }
	|	'(' expr ')'			{ $$.u.ast = $2.u.ast; }
	|	dereferencable_scalar	{ $$.u.ast = $1.u.ast; }
;

callable_expr:
		callable_variable		{ $$.u.ast = $1.u.ast; }
	|	'(' expr ')'			{ $$.u.ast = $2.u.ast; }
	|	dereferencable_scalar	{ $$.u.ast = $1.u.ast; }
;

callable_variable:
		simple_variable
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.u.ast); }
	|	dereferencable '[' dim_offset ']'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.u.ast, $3.u.ast); }
	|	dereferencable '{' expr '}'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.u.ast, $3.u.ast); }
	|	dereferencable T_OBJECT_OPERATOR member_name argument_list
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_METHOD_CALL, $1.u.ast, $3.u.ast, $4.u.ast); }
	|	function_call { $$.u.ast = $1.u.ast; }
;

variable:
		callable_variable
			{ $$.u.ast = $1.u.ast; }
	|	static_member
			{ $$.u.ast = $1.u.ast; }
	|	dereferencable T_OBJECT_OPERATOR member_name
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_PROP, $1.u.ast, $3.u.ast); }
;

simple_variable:
		T_VARIABLE
			{ $$.u.ast = AST_ZVAL(&$1); }
	|	'$' '{' expr '}'
			{ $$.u.ast = $3.u.ast; }
	|	'$' simple_variable
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_VAR, $2.u.ast); }
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.u.ast, $3.u.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.u.ast, $3.u.ast); }
;

new_variable:
		simple_variable
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.u.ast); }
	|	new_variable '[' dim_offset ']'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.u.ast, $3.u.ast); }
	|	new_variable '{' expr '}'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM, $1.u.ast, $3.u.ast); }
	|	new_variable T_OBJECT_OPERATOR member_name
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_PROP, $1.u.ast, $3.u.ast); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.u.ast, $3.u.ast); }
	|	new_variable T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, $1.u.ast, $3.u.ast); }
;

dim_offset:
		/* empty */		{ $$.u.ast = NULL; }
	|	expr			{ $$.u.ast = $1.u.ast; }
;

member_name:
		T_STRING		{ $$.u.ast = AST_ZVAL(&$1); }
	|	'{' expr '}'	{ $$.u.ast = $2.u.ast; }
	|	simple_variable	{ $$.u.ast = zend_ast_create_unary(ZEND_AST_VAR, $1.u.ast); }
;

assignment_list:
		assignment_list ',' assignment_list_element
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	assignment_list_element
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_LIST, $1.u.ast); }
;

assignment_list_element:
		variable						{ $$.u.ast = $1.u.ast; }
	|	T_LIST '(' assignment_list ')'	{ $$.u.ast = $3.u.ast; }
	|	/* empty */						{ $$.u.ast = NULL; }
;


array_pair_list:
		/* empty */ { $$.u.ast = zend_ast_create_dynamic(ZEND_AST_ARRAY); }
	|	non_empty_array_pair_list possible_comma { $$.u.ast = $1.u.ast; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' array_pair
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	array_pair
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_ARRAY, $1.u.ast); }
;

array_pair:
		expr T_DOUBLE_ARROW expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $3.u.ast, $1.u.ast); }
	|	expr { $$.u.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $1.u.ast, NULL); }
	|	expr T_DOUBLE_ARROW '&' variable
			{ $$.u.ast = zend_ast_create_ex(2, ZEND_AST_ARRAY_ELEM, 1, $4.u.ast, $1.u.ast); }
	|	'&' variable
			{ $$.u.ast = zend_ast_create_ex(2, ZEND_AST_ARRAY_ELEM, 1, $2.u.ast, NULL); }
;

encaps_list:
		encaps_list encaps_var
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $2.u.ast); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, AST_ZVAL(&$2)); }
	|	encaps_var
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_ENCAPS_LIST, $1.u.ast); }
	|	T_ENCAPSED_AND_WHITESPACE encaps_var
			{ $$.u.ast = zend_ast_dynamic_add(zend_ast_create_dynamic_and_add(
			      ZEND_AST_ENCAPS_LIST, AST_ZVAL(&$1)), $2.u.ast); }
;

encaps_var:
		T_VARIABLE
			{ $$.u.ast = zend_ast_create_var(&$1.u.constant); }
	|	T_VARIABLE '[' encaps_var_offset ']'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM,
			      zend_ast_create_var(&$1.u.constant), $3.u.ast); }
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_PROP,
			      zend_ast_create_var(&$1.u.constant), AST_ZVAL(&$3)); }
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_VAR, $2.u.ast); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '}'
			{ $$.u.ast = zend_ast_create_var(&$2.u.constant); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_DIM,
			      zend_ast_create_var(&$2.u.constant), $4.u.ast); }
	|	T_CURLY_OPEN variable '}' { $$.u.ast = $2.u.ast; }
;

encaps_var_offset:
		T_STRING		{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_NUM_STRING	{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_VARIABLE		{ $$.u.ast = zend_ast_create_var(&$1.u.constant); }
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables ')' { $$.u.ast  = $3.u.ast; }
	|	T_EMPTY '(' expr ')' { $$.u.ast = zend_ast_create_unary(ZEND_AST_EMPTY, $3.u.ast); }
	|	T_INCLUDE expr
			{ $$.u.ast = zend_ast_create_ex(1, ZEND_INCLUDE_OR_EVAL, ZEND_INCLUDE, $2.u.ast); }
	|	T_INCLUDE_ONCE expr
			{ $$.u.ast = zend_ast_create_ex(1, ZEND_INCLUDE_OR_EVAL, ZEND_INCLUDE_ONCE, $2.u.ast); }
	|	T_EVAL '(' expr ')'
			{ $$.u.ast = zend_ast_create_ex(1, ZEND_INCLUDE_OR_EVAL, ZEND_EVAL, $3.u.ast); }
	|	T_REQUIRE expr
			{ $$.u.ast = zend_ast_create_ex(1, ZEND_INCLUDE_OR_EVAL, ZEND_REQUIRE, $2.u.ast); }
	|	T_REQUIRE_ONCE expr
			{ $$.u.ast = zend_ast_create_ex(1, ZEND_INCLUDE_OR_EVAL, ZEND_REQUIRE_ONCE, $2.u.ast); }
;

isset_variables:
		isset_variable { $$.u.ast = $1.u.ast; }
	|	isset_variables ',' isset_variable
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_AND, $1.u.ast, $3.u.ast); }
;

isset_variable:
		expr { $$.u.ast = zend_ast_create_unary(ZEND_AST_ISSET, $1.u.ast); }
;

class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$.u.ast = zend_ast_create_binary(
			      ZEND_AST_CLASS_CONST, $1.u.ast, AST_ZVAL(&$3)); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$.u.ast = zend_ast_create_binary(
			      ZEND_AST_CLASS_CONST, $1.u.ast, AST_ZVAL(&$3)); }
;

class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS
		{ $$.u.ast = zend_ast_create_unary(ZEND_AST_RESOLVE_CLASS_NAME, $1.u.ast); }
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
