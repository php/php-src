%{
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

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

%}

%pure_parser
%expect 2

%code requires {
}

%destructor { zend_ast_destroy($$); } <ast>
%destructor { if ($$) zend_string_release($$); } <str>

%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%left ','
%left T_LOGICAL_OR
%left T_LOGICAL_XOR
%left T_LOGICAL_AND
%right T_PRINT
%right T_YIELD
%right T_DOUBLE_ARROW
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL T_POW_EQUAL
%left '?' ':'
%right T_COALESCE
%left T_BOOLEAN_OR
%left T_BOOLEAN_AND
%left '|'
%left '^'
%left '&'
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL
%left T_SL T_SR
%left '+' '-' '.'
%left '*' '/' '%'
%right '!'
%nonassoc T_INSTANCEOF
%right '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right T_POW
%right '['
%nonassoc T_NEW T_CLONE
%left T_ELSEIF
%left T_ELSE
%left T_ENDIF
%right T_STATIC T_ABSTRACT T_FINAL T_PRIVATE T_PROTECTED T_PUBLIC

%token <ast> T_LNUMBER   "integer number (T_LNUMBER)"
%token <ast> T_DNUMBER   "floating-point number (T_DNUMBER)"
%token <ast> T_STRING    "identifier (T_STRING)"
%token <ast> T_VARIABLE  "variable (T_VARIABLE)"
%token <ast> T_INLINE_HTML
%token <ast> T_ENCAPSED_AND_WHITESPACE  "quoted-string and whitespace (T_ENCAPSED_AND_WHITESPACE)"
%token <ast> T_CONSTANT_ENCAPSED_STRING "quoted-string (T_CONSTANT_ENCAPSED_STRING)"
%token <ast> T_STRING_VARNAME "variable name (T_STRING_VARNAME)"
%token <ast> T_NUM_STRING "number (T_NUM_STRING)"

%token END 0 "end of file"
%token T_INCLUDE      "include (T_INCLUDE)"
%token T_INCLUDE_ONCE "include_once (T_INCLUDE_ONCE)"
%token T_EVAL         "eval (T_EVAL)"
%token T_REQUIRE      "require (T_REQUIRE)"
%token T_REQUIRE_ONCE "require_once (T_REQUIRE_ONCE)"
%token T_LOGICAL_OR   "or (T_LOGICAL_OR)"
%token T_LOGICAL_XOR  "xor (T_LOGICAL_XOR)"
%token T_LOGICAL_AND  "and (T_LOGICAL_AND)"
%token T_PRINT        "print (T_PRINT)"
%token T_YIELD        "yield (T_YIELD)"
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
%token T_BOOLEAN_OR   "|| (T_BOOLEAN_OR)"
%token T_BOOLEAN_AND  "&& (T_BOOLEAN_AND)"
%token T_IS_EQUAL     "== (T_IS_EQUAL)"
%token T_IS_NOT_EQUAL "!= (T_IS_NOT_EQUAL)"
%token T_IS_IDENTICAL "=== (T_IS_IDENTICAL)"
%token T_IS_NOT_IDENTICAL "!== (T_IS_NOT_IDENTICAL)"
%token T_IS_SMALLER_OR_EQUAL "<= (T_IS_SMALLER_OR_EQUAL)"
%token T_IS_GREATER_OR_EQUAL ">= (T_IS_GREATER_OR_EQUAL)"
%token T_SL "<< (T_SL)"
%token T_SR ">> (T_SR)"
%token T_INSTANCEOF  "instanceof (T_INSTANCEOF)"
%token T_INC "++ (T_INC)"
%token T_DEC "-- (T_DEC)"
%token T_INT_CAST    "(int) (T_INT_CAST)"
%token T_DOUBLE_CAST "(double) (T_DOUBLE_CAST)"
%token T_STRING_CAST "(string) (T_STRING_CAST)"
%token T_ARRAY_CAST  "(array) (T_ARRAY_CAST)"
%token T_OBJECT_CAST "(object) (T_OBJECT_CAST)"
%token T_BOOL_CAST   "(bool) (T_BOOL_CAST)"
%token T_UNSET_CAST  "(unset) (T_UNSET_CAST)"
%token T_NEW       "new (T_NEW)"
%token T_CLONE     "clone (T_CLONE)"
%token T_EXIT      "exit (T_EXIT)"
%token T_IF        "if (T_IF)"
%token T_ELSEIF    "elseif (T_ELSEIF)"
%token T_ELSE      "else (T_ELSE)"
%token T_ENDIF     "endif (T_ENDIF)"
%token T_CHARACTER
%token T_BAD_CHARACTER
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
%token T_DOUBLE_ARROW    "=> (T_DOUBLE_ARROW)"
%token T_LIST            "list (T_LIST)"
%token T_ARRAY           "array (T_ARRAY)"
%token T_CALLABLE        "callable (T_CALLABLE)"
%token T_LINE            "__LINE__ (T_LINE)"
%token T_FILE            "__FILE__ (T_FILE)"
%token T_DIR             "__DIR__ (T_DIR)"
%token T_CLASS_C         "__CLASS__ (T_CLASS_C)"
%token T_TRAIT_C         "__TRAIT__ (T_TRAIT_C)"
%token T_METHOD_C        "__METHOD__ (T_METHOD_C)"
%token T_FUNC_C          "__FUNCTION__ (T_FUNC_C)"
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
%token T_NS_SEPARATOR    "\\ (T_NS_SEPARATOR)"
%token T_ELLIPSIS        "... (T_ELLIPSIS)"
%token T_COALESCE        "?? (T_COALESCE)"
%token T_POW             "** (T_POW)"
%token T_POW_EQUAL       "**= (T_POW_EQUAL)"

%type <ast> top_statement namespace_name name statement function_declaration_statement
%type <ast> class_declaration_statement trait_declaration_statement
%type <ast> interface_declaration_statement interface_extends_list
%type <ast> use_declaration const_decl inner_statement
%type <ast> expr optional_expr while_statement for_statement foreach_variable
%type <ast> foreach_statement declare_statement finally_statement unset_variable variable
%type <ast> extends_from parameter optional_type argument expr_without_variable global_var
%type <ast> static_var class_statement trait_adaptation trait_precedence trait_alias
%type <ast> absolute_trait_method_reference trait_method_reference property echo_expr
%type <ast> new_expr class_name class_name_reference simple_variable internal_functions_in_yacc
%type <ast> exit_expr scalar backticks_expr lexical_var function_call member_name
%type <ast> variable_class_name dereferencable_scalar class_name_scalar constant dereferencable
%type <ast> callable_expr callable_variable static_member new_variable
%type <ast> assignment_list_element array_pair encaps_var encaps_var_offset isset_variables
%type <ast> top_statement_list use_declarations const_list inner_statement_list if_stmt
%type <ast> alt_if_stmt for_exprs switch_case_list global_var_list static_var_list
%type <ast> echo_expr_list unset_variables catch_list parameter_list class_statement_list
%type <ast> implements_list case_list if_stmt_without_else
%type <ast> non_empty_parameter_list argument_list non_empty_argument_list property_list
%type <ast> class_const_list name_list trait_adaptations method_body non_empty_for_exprs
%type <ast> ctor_arguments alt_if_stmt_without_else trait_adaptation_list lexical_vars
%type <ast> lexical_var_list encaps_list array_pair_list non_empty_array_pair_list
%type <ast> assignment_list isset_variable type return_type

%type <num> returns_ref function is_reference is_variadic variable_modifiers
%type <num> method_modifiers trait_modifiers non_empty_member_modifiers member_modifier
%type <num> class_modifiers class_modifier

%type <str> backup_doc_comment

%% /* Rules */

start:
	top_statement_list	{ CG(ast) = $1; }
;

top_statement_list:
		top_statement_list top_statement { $$ = zend_ast_list_add($1, $2); }
	|	/* empty */ { $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;

namespace_name:
		T_STRING								{ $$ = $1; }
	|	namespace_name T_NS_SEPARATOR T_STRING	{ $$ = zend_ast_append_str($1, $3); }
;

name:
		namespace_name								{ $$ = $1; $$->attr = ZEND_NAME_NOT_FQ; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name	{ $$ = $3; $$->attr = ZEND_NAME_RELATIVE; }
	|	T_NS_SEPARATOR namespace_name				{ $$ = $2; $$->attr = ZEND_NAME_FQ; }
;

top_statement:
		statement							{ $$ = $1; }
	|	function_declaration_statement		{ $$ = $1; }
	|	class_declaration_statement			{ $$ = $1; }
	|	trait_declaration_statement			{ $$ = $1; }
	|	interface_declaration_statement		{ $$ = $1; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ $$ = zend_ast_create(ZEND_AST_HALT_COMPILER,
			      zend_ast_create_zval_from_long(zend_get_scanned_file_offset()));
			  zend_stop_lexing(); }
	|	T_NAMESPACE namespace_name ';'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, $2, NULL);
			  RESET_DOC_COMMENT(); }
	|	T_NAMESPACE namespace_name { RESET_DOC_COMMENT(); }
		'{' top_statement_list '}'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, $2, $5); }
	|	T_NAMESPACE { RESET_DOC_COMMENT(); }
		'{' top_statement_list '}'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, NULL, $4); }
	|	T_USE use_declarations ';'				{ $$ = $2; $$->attr = T_CLASS; }
	|	T_USE T_FUNCTION use_declarations ';'	{ $$ = $3; $$->attr = T_FUNCTION; }
	|	T_USE T_CONST use_declarations ';'		{ $$ = $3; $$->attr = T_CONST; }
	|	T_CONST const_list ';'					{ $$ = $2; }
;

use_declarations:
		use_declarations ',' use_declaration
			{ $$ = zend_ast_list_add($1, $3); }
	|	use_declaration
			{ $$ = zend_ast_create_list(1, ZEND_AST_USE, $1); }
;

use_declaration:
		namespace_name
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, NULL); }
	|	namespace_name T_AS T_STRING
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, $3); }
	|	T_NS_SEPARATOR namespace_name
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $2, NULL); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $2, $4); }
;

const_list:
		const_list ',' const_decl { $$ = zend_ast_list_add($1, $3); }
	|	const_decl { $$ = zend_ast_create_list(1, ZEND_AST_CONST_DECL, $1); }
;

inner_statement_list:
		inner_statement_list inner_statement
			{ $$ = zend_ast_list_add($1, $2); }
	|	/* empty */
			{ $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


inner_statement:
		statement { $$ = $1; }
	|	function_declaration_statement 		{ $$ = $1; }
	|	class_declaration_statement 		{ $$ = $1; }
	|	trait_declaration_statement			{ $$ = $1; }
	|	interface_declaration_statement		{ $$ = $1; }
	|	T_HALT_COMPILER '(' ')' ';'
			{ $$ = NULL; zend_error_noreturn(E_COMPILE_ERROR,
			      "__HALT_COMPILER() can only be used from the outermost scope"); }
;


statement:
		'{' inner_statement_list '}' { $$ = $2; }
	|	if_stmt { $$ = $1; }
	|	alt_if_stmt { $$ = $1; }
	|	T_WHILE '(' expr ')' while_statement
			{ $$ = zend_ast_create(ZEND_AST_WHILE, $3, $5); }
	|	T_DO statement T_WHILE '(' expr ')' ';'
			{ $$ = zend_ast_create(ZEND_AST_DO_WHILE, $2, $5); }
	|	T_FOR '(' for_exprs ';' for_exprs ';' for_exprs ')' for_statement
			{ $$ = zend_ast_create(ZEND_AST_FOR, $3, $5, $7, $9); }
	|	T_SWITCH '(' expr ')' switch_case_list
			{ $$ = zend_ast_create(ZEND_AST_SWITCH, $3, $5); }
	|	T_BREAK optional_expr ';'		{ $$ = zend_ast_create(ZEND_AST_BREAK, $2); }
	|	T_CONTINUE optional_expr ';'	{ $$ = zend_ast_create(ZEND_AST_CONTINUE, $2); }
	|	T_RETURN optional_expr ';'		{ $$ = zend_ast_create(ZEND_AST_RETURN, $2); }
	|	T_GLOBAL global_var_list ';'	{ $$ = $2; }
	|	T_STATIC static_var_list ';'	{ $$ = $2; }
	|	T_ECHO echo_expr_list ';'		{ $$ = $2; }
	|	T_INLINE_HTML { $$ = zend_ast_create(ZEND_AST_ECHO, $1); }
	|	expr ';' { $$ = $1; }
	|	T_UNSET '(' unset_variables ')' ';' { $$ = $3; }
	|	T_FOREACH '(' expr T_AS foreach_variable ')' foreach_statement
			{ $$ = zend_ast_create(ZEND_AST_FOREACH, $3, $5, NULL, $7); }
	|	T_FOREACH '(' expr T_AS foreach_variable T_DOUBLE_ARROW foreach_variable ')'
		foreach_statement
			{ $$ = zend_ast_create(ZEND_AST_FOREACH, $3, $7, $5, $9); }
	|	T_DECLARE '(' const_list ')'
			{ zend_handle_encoding_declaration($3); }
		declare_statement
			{ $$ = zend_ast_create(ZEND_AST_DECLARE, $3, $6); }
	|	';'	/* empty statement */ { $$ = NULL; }
	|	T_TRY '{' inner_statement_list '}' catch_list finally_statement
			{ $$ = zend_ast_create(ZEND_AST_TRY, $3, $5, $6); }
	|	T_THROW expr ';' { $$ = zend_ast_create(ZEND_AST_THROW, $2); }
	|	T_GOTO T_STRING ';' { $$ = zend_ast_create(ZEND_AST_GOTO, $2); }
	|	T_STRING ':' { $$ = zend_ast_create(ZEND_AST_LABEL, $1); }
;

catch_list:
		/* empty */
			{ $$ = zend_ast_create_list(0, ZEND_AST_CATCH_LIST); }
	|	catch_list T_CATCH '(' name T_VARIABLE ')' '{' inner_statement_list '}'
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_CATCH, $4, $5, $8)); }
;

finally_statement:
		/* empty */ { $$ = NULL; }
	|	T_FINALLY '{' inner_statement_list '}' { $$ = $3; }
;

unset_variables:
		unset_variable { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
	|	unset_variables ',' unset_variable { $$ = zend_ast_list_add($1, $3); }
;

unset_variable:
		variable { $$ = zend_ast_create(ZEND_AST_UNSET, $1); }
;

function_declaration_statement:
	function returns_ref T_STRING '(' parameter_list ')' return_type
	backup_doc_comment '{' inner_statement_list '}'
		{ $$ = zend_ast_create_decl(ZEND_AST_FUNC_DECL, $2, $1, $8,
		      zend_ast_get_str($3), $5, NULL, $10, $7); }
;

is_reference:
		/* empty */	{ $$ = 0; }
	|	'&'			{ $$ = ZEND_PARAM_REF; }
;

is_variadic:
		/* empty */ { $$ = 0; }
	|	T_ELLIPSIS  { $$ = ZEND_PARAM_VARIADIC; }
;

class_declaration_statement:
		class_modifiers T_CLASS { $<num>$ = CG(zend_lineno); }
		T_STRING extends_from implements_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, $1, $<num>3, $7, zend_ast_get_str($4), $5, $6, $9, NULL); }
	|	T_CLASS { $<num>$ = CG(zend_lineno); }
		T_STRING extends_from implements_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, 0, $<num>2, $6, zend_ast_get_str($3), $4, $5, $8, NULL); }
;

class_modifiers:
		class_modifier 					{ $$ = $1; }
	|	class_modifiers class_modifier 	{ $$ = zend_add_class_modifier($1, $2); }
;

class_modifier:
		T_ABSTRACT 		{ $$ = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
	|	T_FINAL 		{ $$ = ZEND_ACC_FINAL; }
;

trait_declaration_statement:
		T_TRAIT { $<num>$ = CG(zend_lineno); }
		T_STRING backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_TRAIT, $<num>2, $4, zend_ast_get_str($3), NULL, NULL, $6, NULL); }
;

interface_declaration_statement:
		T_INTERFACE { $<num>$ = CG(zend_lineno); }
		T_STRING interface_extends_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_INTERFACE, $<num>2, $5, zend_ast_get_str($3), NULL, $4, $7, NULL); }
;

extends_from:
		/* empty */		{ $$ = NULL; }
	|	T_EXTENDS name	{ $$ = $2; }
;

interface_extends_list:
		/* empty */			{ $$ = NULL; }
	|	T_EXTENDS name_list	{ $$ = $2; }
;

implements_list:
		/* empty */				{ $$ = NULL; }
	|	T_IMPLEMENTS name_list	{ $$ = $2; }
;

foreach_variable:
		variable			{ $$ = $1; }
	|	'&' variable		{ $$ = zend_ast_create(ZEND_AST_REF, $2); }
	|	T_LIST '(' assignment_list ')' { $$ = $3; }
;

for_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDFOR ';' { $$ = $2; }
;

foreach_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDFOREACH ';' { $$ = $2; }
;

declare_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDDECLARE ';' { $$ = $2; }
;

switch_case_list:
		'{' case_list '}'					{ $$ = $2; }
	|	'{' ';' case_list '}'				{ $$ = $3; }
	|	':' case_list T_ENDSWITCH ';'		{ $$ = $2; }
	|	':' ';' case_list T_ENDSWITCH ';'	{ $$ = $3; }
;

case_list:
		/* empty */ { $$ = zend_ast_create_list(0, ZEND_AST_SWITCH_LIST); }
	|	case_list T_CASE expr case_separator inner_statement_list
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_SWITCH_CASE, $3, $5)); }
	|	case_list T_DEFAULT case_separator inner_statement_list
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_SWITCH_CASE, NULL, $4)); }
;

case_separator:
		':'
	|	';'
;


while_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDWHILE ';' { $$ = $2; }
;


if_stmt_without_else:
		T_IF '(' expr ')' statement
			{ $$ = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create(ZEND_AST_IF_ELEM, $3, $5)); }
	|	if_stmt_without_else T_ELSEIF '(' expr ')' statement
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, $4, $6)); }
;

if_stmt:
		if_stmt_without_else { $$ = $1; }
	|	if_stmt_without_else T_ELSE statement
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_IF_ELEM, NULL, $3)); }
;

alt_if_stmt_without_else:
		T_IF '(' expr ')' ':' inner_statement_list
			{ $$ = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create(ZEND_AST_IF_ELEM, $3, $6)); }
	|	alt_if_stmt_without_else T_ELSEIF '(' expr ')' ':' inner_statement_list
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, $4, $7)); }
;

alt_if_stmt:
		alt_if_stmt_without_else T_ENDIF ';' { $$ = $1; }
	|	alt_if_stmt_without_else T_ELSE ':' inner_statement_list T_ENDIF ';'
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, NULL, $4)); }
;

parameter_list:
		non_empty_parameter_list { $$ = $1; }
	|	/* empty */	{ $$ = zend_ast_create_list(0, ZEND_AST_PARAM_LIST); }
;


non_empty_parameter_list:
		parameter
			{ $$ = zend_ast_create_list(1, ZEND_AST_PARAM_LIST, $1); }
	|	non_empty_parameter_list ',' parameter
			{ $$ = zend_ast_list_add($1, $3); }
;

parameter:
		optional_type is_reference is_variadic T_VARIABLE
			{ $$ = zend_ast_create_ex(ZEND_AST_PARAM, $2 | $3, $1, $4, NULL); }
	|	optional_type is_reference is_variadic T_VARIABLE '=' expr
			{ $$ = zend_ast_create_ex(ZEND_AST_PARAM, $2 | $3, $1, $4, $6); }
;


optional_type:
		/* empty */	{ $$ = NULL; }
	|	type		{ $$ = $1; }
;

type:
		T_ARRAY		{ $$ = zend_ast_create_ex(ZEND_AST_TYPE, IS_ARRAY); }
	|	T_CALLABLE	{ $$ = zend_ast_create_ex(ZEND_AST_TYPE, IS_CALLABLE); }
	|	name		{ $$ = $1; }
;

return_type:
		/* empty */	{ $$ = NULL; }
	|	':' type	{ $$ = $2; }
;

argument_list:
		'(' ')'	{ $$ = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	'(' non_empty_argument_list ')' { $$ = $2; }
;

non_empty_argument_list:
		argument
			{ $$ = zend_ast_create_list(1, ZEND_AST_ARG_LIST, $1); }
	|	non_empty_argument_list ',' argument
			{ $$ = zend_ast_list_add($1, $3); }
;

argument:
		expr			{ $$ = $1; }
	|	T_ELLIPSIS expr	{ $$ = zend_ast_create(ZEND_AST_UNPACK, $2); }
;

global_var_list:
		global_var_list ',' global_var { $$ = zend_ast_list_add($1, $3); }
	|	global_var { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;

global_var:
	simple_variable
		{ $$ = zend_ast_create(ZEND_AST_GLOBAL, zend_ast_create(ZEND_AST_VAR, $1)); }
;


static_var_list:
		static_var_list ',' static_var { $$ = zend_ast_list_add($1, $3); }
	|	static_var { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;

static_var:
		T_VARIABLE			{ $$ = zend_ast_create(ZEND_AST_STATIC, $1, NULL); }
	|	T_VARIABLE '=' expr	{ $$ = zend_ast_create(ZEND_AST_STATIC, $1, $3); }
;


class_statement_list:
		class_statement_list class_statement
			{ $$ = zend_ast_list_add($1, $2); }
	|	/* empty */
			{ $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


class_statement:
		variable_modifiers property_list ';'
			{ $$ = zend_ast_append_doc_comment($2); $$->attr = $1; }
	|	T_CONST class_const_list ';'
			{ $$ = $2; RESET_DOC_COMMENT(); }
	|	T_USE name_list trait_adaptations
			{ $$ = zend_ast_create(ZEND_AST_USE_TRAIT, $2, $3); }
	|	method_modifiers function returns_ref T_STRING '(' parameter_list ')'
		return_type backup_doc_comment method_body
			{ $$ = zend_ast_create_decl(ZEND_AST_METHOD, $3 | $1, $2, $9,
				  zend_ast_get_str($4), $6, NULL, $10, $8); }
;

name_list:
		name { $$ = zend_ast_create_list(1, ZEND_AST_NAME_LIST, $1); }
	|	name_list ',' name { $$ = zend_ast_list_add($1, $3); }
;

trait_adaptations:
		';'								{ $$ = NULL; }
	|	'{' '}'							{ $$ = NULL; }
	|	'{' trait_adaptation_list '}'	{ $$ = $2; }
;

trait_adaptation_list:
		trait_adaptation
			{ $$ = zend_ast_create_list(1, ZEND_AST_TRAIT_ADAPTATIONS, $1); }
	|	trait_adaptation_list trait_adaptation
			{ $$ = zend_ast_list_add($1, $2); }
;

trait_adaptation:
		trait_precedence ';'	{ $$ = $1; }
	|	trait_alias ';'			{ $$ = $1; }
;

trait_precedence:
	absolute_trait_method_reference T_INSTEADOF name_list
		{ $$ = zend_ast_create(ZEND_AST_TRAIT_PRECEDENCE, $1, $3); }
;

trait_alias:
		trait_method_reference T_AS trait_modifiers T_STRING
			{ $$ = zend_ast_create_ex(ZEND_AST_TRAIT_ALIAS, $3, $1, $4); }
	|	trait_method_reference T_AS member_modifier
			{ $$ = zend_ast_create_ex(ZEND_AST_TRAIT_ALIAS, $3, $1, NULL); }
;

trait_method_reference:
		T_STRING
			{ $$ = zend_ast_create(ZEND_AST_METHOD_REFERENCE, NULL, $1); }
	|	absolute_trait_method_reference { $$ = $1; }
;

absolute_trait_method_reference:
	name T_PAAMAYIM_NEKUDOTAYIM T_STRING
		{ $$ = zend_ast_create(ZEND_AST_METHOD_REFERENCE, $1, $3); }
;

trait_modifiers:
		/* empty */		{ $$ = 0; }
	|	member_modifier	{ $$ = $1; }
;

method_body:
		';' /* abstract method */		{ $$ = NULL; }
	|	'{' inner_statement_list '}'	{ $$ = $2; }
;

variable_modifiers:
		non_empty_member_modifiers		{ $$ = $1; }
	|	T_VAR							{ $$ = ZEND_ACC_PUBLIC; }
;

method_modifiers:
		/* empty */						{ $$ = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers
			{ $$ = $1; if (!($$ & ZEND_ACC_PPP_MASK)) { $$ |= ZEND_ACC_PUBLIC; } }
;

non_empty_member_modifiers:
		member_modifier			{ $$ = $1; }
	|	non_empty_member_modifiers member_modifier
			{ $$ = zend_add_member_modifier($1, $2); }
;

member_modifier:
		T_PUBLIC				{ $$ = ZEND_ACC_PUBLIC; }
	|	T_PROTECTED				{ $$ = ZEND_ACC_PROTECTED; }
	|	T_PRIVATE				{ $$ = ZEND_ACC_PRIVATE; }
	|	T_STATIC				{ $$ = ZEND_ACC_STATIC; }
	|	T_ABSTRACT				{ $$ = ZEND_ACC_ABSTRACT; }
	|	T_FINAL					{ $$ = ZEND_ACC_FINAL; }
;

property_list:
		property_list ',' property { $$ = zend_ast_list_add($1, $3); }
	|	property { $$ = zend_ast_create_list(1, ZEND_AST_PROP_DECL, $1); }
;

property:
		T_VARIABLE { $$ = zend_ast_create(ZEND_AST_PROP_ELEM, $1, NULL); }
	|	T_VARIABLE '=' expr
			{ $$ = zend_ast_create(ZEND_AST_PROP_ELEM, $1, $3); }
;

class_const_list:
		class_const_list ',' const_decl { $$ = zend_ast_list_add($1, $3); }
	|	const_decl { $$ = zend_ast_create_list(1, ZEND_AST_CLASS_CONST_DECL, $1); }
;

const_decl:
	T_STRING '=' expr { $$ = zend_ast_create(ZEND_AST_CONST_ELEM, $1, $3); }
;

echo_expr_list:
		echo_expr_list ',' echo_expr { $$ = zend_ast_list_add($1, $3); }
	|	echo_expr { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;
echo_expr:
	expr { $$ = zend_ast_create(ZEND_AST_ECHO, $1); }
;

for_exprs:
		/* empty */			{ $$ = NULL; }
	|	non_empty_for_exprs	{ $$ = $1; }
;

non_empty_for_exprs:
		non_empty_for_exprs ',' expr { $$ = zend_ast_list_add($1, $3); }
	|	expr { $$ = zend_ast_create_list(1, ZEND_AST_EXPR_LIST, $1); }
;

new_expr:
		T_NEW class_name_reference ctor_arguments
			{ $$ = zend_ast_create(ZEND_AST_NEW, $2, $3); }
;

expr_without_variable:
		T_LIST '(' assignment_list ')' '=' expr
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN, $3, $6); }
	|	variable '=' expr
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN, $1, $3); }
	|	variable '=' '&' variable
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN_REF, $1, $4); }
	|	T_CLONE expr { $$ = zend_ast_create(ZEND_AST_CLONE, $2); }
	|	variable T_PLUS_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_ADD, $1, $3); }
	|	variable T_MINUS_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_SUB, $1, $3); }
	|	variable T_MUL_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_MUL, $1, $3); }
	|	variable T_POW_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_POW, $1, $3); }
	|	variable T_DIV_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_DIV, $1, $3); }
	|	variable T_CONCAT_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_CONCAT, $1, $3); }
	|	variable T_MOD_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_MOD, $1, $3); }
	|	variable T_AND_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_BW_AND, $1, $3); }
	|	variable T_OR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_BW_OR, $1, $3); }
	|	variable T_XOR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_BW_XOR, $1, $3); }
	|	variable T_SL_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_SL, $1, $3); }
	|	variable T_SR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ASSIGN_SR, $1, $3); }
	|	variable T_INC { $$ = zend_ast_create(ZEND_AST_POST_INC, $1); }
	|	T_INC variable { $$ = zend_ast_create(ZEND_AST_PRE_INC, $2); }
	|	variable T_DEC { $$ = zend_ast_create(ZEND_AST_POST_DEC, $1); }
	|	T_DEC variable { $$ = zend_ast_create(ZEND_AST_PRE_DEC, $2); }
	|	expr T_BOOLEAN_OR expr
			{ $$ = zend_ast_create(ZEND_AST_OR, $1, $3); }
	|	expr T_BOOLEAN_AND expr
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
	|	expr T_LOGICAL_OR expr
			{ $$ = zend_ast_create(ZEND_AST_OR, $1, $3); }
	|	expr T_LOGICAL_AND expr
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
	|	expr T_LOGICAL_XOR expr
			{ $$ = zend_ast_create_binary_op(ZEND_BOOL_XOR, $1, $3); }
	|	expr '|' expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_OR, $1, $3); }
	|	expr '&' expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_AND, $1, $3); }
	|	expr '^' expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_XOR, $1, $3); }
	|	expr '.' expr 	{ $$ = zend_ast_create_binary_op(ZEND_CONCAT, $1, $3); }
	|	expr '+' expr 	{ $$ = zend_ast_create_binary_op(ZEND_ADD, $1, $3); }
	|	expr '-' expr 	{ $$ = zend_ast_create_binary_op(ZEND_SUB, $1, $3); }
	|	expr '*' expr	{ $$ = zend_ast_create_binary_op(ZEND_MUL, $1, $3); }
	|	expr T_POW expr	{ $$ = zend_ast_create_binary_op(ZEND_POW, $1, $3); }
	|	expr '/' expr	{ $$ = zend_ast_create_binary_op(ZEND_DIV, $1, $3); }
	|	expr '%' expr 	{ $$ = zend_ast_create_binary_op(ZEND_MOD, $1, $3); }
	| 	expr T_SL expr	{ $$ = zend_ast_create_binary_op(ZEND_SL, $1, $3); }
	|	expr T_SR expr	{ $$ = zend_ast_create_binary_op(ZEND_SR, $1, $3); }
	|	'+' expr %prec T_INC { $$ = zend_ast_create(ZEND_AST_UNARY_PLUS, $2); }
	|	'-' expr %prec T_INC { $$ = zend_ast_create(ZEND_AST_UNARY_MINUS, $2); }
	|	'!' expr { $$ = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BOOL_NOT, $2); }
	|	'~' expr { $$ = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BW_NOT, $2); }
	|	expr T_IS_IDENTICAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_IDENTICAL, $1, $3); }
	|	expr T_IS_NOT_IDENTICAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_NOT_IDENTICAL, $1, $3); }
	|	expr T_IS_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_EQUAL, $1, $3); }
	|	expr T_IS_NOT_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_NOT_EQUAL, $1, $3); }
	|	expr '<' expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1, $3); }
	|	expr T_IS_SMALLER_OR_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $1, $3); }
	|	expr '>' expr
			{ $$ = zend_ast_create(ZEND_AST_GREATER, $1, $3); }
	|	expr T_IS_GREATER_OR_EQUAL expr
			{ $$ = zend_ast_create(ZEND_AST_GREATER_EQUAL, $1, $3); }
	|	expr T_INSTANCEOF class_name_reference
			{ $$ = zend_ast_create(ZEND_AST_INSTANCEOF, $1, $3); }
	|	'(' expr ')' { $$ = $2; }
	|	new_expr { $$ = $1; }
	|	expr '?' expr ':' expr
			{ $$ = zend_ast_create(ZEND_AST_CONDITIONAL, $1, $3, $5); }
	|	expr '?' ':' expr
			{ $$ = zend_ast_create(ZEND_AST_CONDITIONAL, $1, NULL, $4); }
	|	expr T_COALESCE expr
			{ $$ = zend_ast_create(ZEND_AST_COALESCE, $1, $3); }
	|	internal_functions_in_yacc { $$ = $1; }
	|	T_INT_CAST expr		{ $$ = zend_ast_create_cast(IS_LONG, $2); }
	|	T_DOUBLE_CAST expr	{ $$ = zend_ast_create_cast(IS_DOUBLE, $2); }
	|	T_STRING_CAST expr	{ $$ = zend_ast_create_cast(IS_STRING, $2); }
	|	T_ARRAY_CAST expr	{ $$ = zend_ast_create_cast(IS_ARRAY, $2); }
	|	T_OBJECT_CAST expr	{ $$ = zend_ast_create_cast(IS_OBJECT, $2); }
	|	T_BOOL_CAST expr	{ $$ = zend_ast_create_cast(_IS_BOOL, $2); }
	|	T_UNSET_CAST expr	{ $$ = zend_ast_create_cast(IS_NULL, $2); }
	|	T_EXIT exit_expr	{ $$ = zend_ast_create(ZEND_AST_EXIT, $2); }
	|	'@' expr			{ $$ = zend_ast_create(ZEND_AST_SILENCE, $2); }
	|	scalar { $$ = $1; }
	|	'`' backticks_expr '`' { $$ = zend_ast_create(ZEND_AST_SHELL_EXEC, $2); }
	|	T_PRINT expr { $$ = zend_ast_create(ZEND_AST_PRINT, $2); }
	|	T_YIELD { $$ = zend_ast_create(ZEND_AST_YIELD, NULL, NULL); }
	|	T_YIELD expr { $$ = zend_ast_create(ZEND_AST_YIELD, $2, NULL); }
	|	T_YIELD expr T_DOUBLE_ARROW expr { $$ = zend_ast_create(ZEND_AST_YIELD, $4, $2); }
	|	function returns_ref '(' parameter_list ')' lexical_vars return_type
		backup_doc_comment '{' inner_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLOSURE, $2, $1, $8,
				  zend_string_init("{closure}", sizeof("{closure}") - 1, 0),
			      $4, $6, $10, $7); }
	|	T_STATIC function returns_ref '(' parameter_list ')' lexical_vars
		return_type backup_doc_comment '{' inner_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLOSURE, $3 | ZEND_ACC_STATIC, $2, $9,
			      zend_string_init("{closure}", sizeof("{closure}") - 1, 0),
			      $5, $7, $11, $8); }
;

function:
	T_FUNCTION { $$ = CG(zend_lineno); }
;

backup_doc_comment:
	/* empty */ { $$ = CG(doc_comment); CG(doc_comment) = NULL; }
;

returns_ref:
		/* empty */	{ $$ = 0; }
	|	'&'			{ $$ = ZEND_ACC_RETURN_REFERENCE; }
;

lexical_vars:
		/* empty */ { $$ = NULL; }
	|	T_USE '(' lexical_var_list ')' { $$ = $3; }
;

lexical_var_list:
		lexical_var_list ',' lexical_var { $$ = zend_ast_list_add($1, $3); }
	|	lexical_var { $$ = zend_ast_create_list(1, ZEND_AST_CLOSURE_USES, $1); }
;

lexical_var:
		T_VARIABLE		{ $$ = $1; }
	|	'&' T_VARIABLE	{ $$ = $2; $$->attr = 1; }
;

function_call:
		name argument_list
			{ $$ = zend_ast_create(ZEND_AST_CALL, $1, $2); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_STATIC_CALL, $1, $3, $4); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_STATIC_CALL, $1, $3, $4); }
	|	callable_expr argument_list
			{ $$ = zend_ast_create(ZEND_AST_CALL, $1, $2); }
;

class_name:
		T_STATIC
			{ zval zv; ZVAL_STRINGL(&zv, "static", sizeof("static")-1);
			  $$ = zend_ast_create_zval_ex(&zv, ZEND_NAME_NOT_FQ); }
	|	name { $$ = $1; }
;

class_name_reference:
		class_name		{ $$ = $1; }
	|	new_variable	{ $$ = $1; }
;

exit_expr:
		/* empty */				{ $$ = NULL; }
	|	'(' optional_expr ')'	{ $$ = $2; }
;

backticks_expr:
		/* empty */
			{ $$ = zend_ast_create_zval_from_str(STR_EMPTY_ALLOC()); }
	|	T_ENCAPSED_AND_WHITESPACE { $$ = $1; }
	|	encaps_list { $$ = $1; }
;


ctor_arguments:
		/* empty */	{ $$ = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	argument_list { $$ = $1; }
;


dereferencable_scalar:
		T_ARRAY '(' array_pair_list ')'	{ $$ = $3; }
	|	'[' array_pair_list ']'			{ $$ = $2; }
	|	T_CONSTANT_ENCAPSED_STRING		{ $$ = $1; }
;

scalar:
		T_LNUMBER 	{ $$ = $1; }
	|	T_DNUMBER 	{ $$ = $1; }
	|	T_LINE 		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_LINE); }
	|	T_FILE 		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_FILE); }
	|	T_DIR   	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_DIR); }
	|	T_TRAIT_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_TRAIT_C); }
	|	T_METHOD_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_METHOD_C); }
	|	T_FUNC_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_FUNC_C); }
	|	T_NS_C		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_NS_C); }
	|	T_CLASS_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_CLASS_C); }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$ = $2; }
	|	T_START_HEREDOC T_END_HEREDOC
			{ $$ = zend_ast_create_zval_from_str(STR_EMPTY_ALLOC()); }
	|	'"' encaps_list '"' 	{ $$ = $2; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$ = $2; }
	|	dereferencable_scalar	{ $$ = $1; }
	|	class_name_scalar	{ $$ = $1; }
	|	constant			{ $$ = $1; }
;

constant:
		name { $$ = zend_ast_create(ZEND_AST_CONST, $1); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST, $1, $3); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST, $1, $3); }
;

possible_comma:
		/* empty */
	|	','
;

expr:
		variable					{ $$ = $1; }
	|	expr_without_variable		{ $$ = $1; }
;

optional_expr:
		/* empty */	{ $$ = NULL; }
	|	expr		{ $$ = $1; }
;

variable_class_name:
	dereferencable { $$ = $1; }
;

dereferencable:
		variable				{ $$ = $1; }
	|	'(' expr ')'			{ $$ = $2; }
	|	dereferencable_scalar	{ $$ = $1; }
;

callable_expr:
		callable_variable		{ $$ = $1; }
	|	'(' expr ')'			{ $$ = $2; }
	|	dereferencable_scalar	{ $$ = $1; }
;

callable_variable:
		simple_variable
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	dereferencable '[' optional_expr ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	constant '[' optional_expr ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	dereferencable '{' expr '}'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	dereferencable T_OBJECT_OPERATOR member_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_METHOD_CALL, $1, $3, $4); }
	|	function_call { $$ = $1; }
;

variable:
		callable_variable
			{ $$ = $1; }
	|	static_member
			{ $$ = $1; }
	|	dereferencable T_OBJECT_OPERATOR member_name
			{ $$ = zend_ast_create(ZEND_AST_PROP, $1, $3); }
;

simple_variable:
		T_VARIABLE			{ $$ = $1; }
	|	'$' '{' expr '}'	{ $$ = $3; }
	|	'$' simple_variable	{ $$ = zend_ast_create(ZEND_AST_VAR, $2); }
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
;

new_variable:
		simple_variable
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	new_variable '[' optional_expr ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	new_variable '{' expr '}'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	new_variable T_OBJECT_OPERATOR member_name
			{ $$ = zend_ast_create(ZEND_AST_PROP, $1, $3); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
	|	new_variable T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
;

member_name:
		T_STRING		{ $$ = $1; }
	|	'{' expr '}'	{ $$ = $2; }
	|	simple_variable	{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
;

assignment_list:
		assignment_list ',' assignment_list_element
			{ $$ = zend_ast_list_add($1, $3); }
	|	assignment_list_element
			{ $$ = zend_ast_create_list(1, ZEND_AST_LIST, $1); }
;

assignment_list_element:
		variable						{ $$ = $1; }
	|	T_LIST '(' assignment_list ')'	{ $$ = $3; }
	|	/* empty */						{ $$ = NULL; }
;


array_pair_list:
		/* empty */ { $$ = zend_ast_create_list(0, ZEND_AST_ARRAY); }
	|	non_empty_array_pair_list possible_comma { $$ = $1; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' array_pair
			{ $$ = zend_ast_list_add($1, $3); }
	|	array_pair
			{ $$ = zend_ast_create_list(1, ZEND_AST_ARRAY, $1); }
;

array_pair:
		expr T_DOUBLE_ARROW expr
			{ $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $3, $1); }
	|	expr { $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $1, NULL); }
	|	expr T_DOUBLE_ARROW '&' variable
			{ $$ = zend_ast_create_ex(ZEND_AST_ARRAY_ELEM, 1, $4, $1); }
	|	'&' variable
			{ $$ = zend_ast_create_ex(ZEND_AST_ARRAY_ELEM, 1, $2, NULL); }
;

encaps_list:
		encaps_list encaps_var
			{ $$ = zend_ast_list_add($1, $2); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE
			{ $$ = zend_ast_list_add($1, $2); }
	|	encaps_var
			{ $$ = zend_ast_create_list(1, ZEND_AST_ENCAPS_LIST, $1); }
	|	T_ENCAPSED_AND_WHITESPACE encaps_var
			{ $$ = zend_ast_create_list(2, ZEND_AST_ENCAPS_LIST, $1, $2); }
;

encaps_var:
		T_VARIABLE
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	T_VARIABLE '[' encaps_var_offset ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM,
			      zend_ast_create(ZEND_AST_VAR, $1), $3); }
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING
			{ $$ = zend_ast_create(ZEND_AST_PROP,
			      zend_ast_create(ZEND_AST_VAR, $1), $3); }
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
			{ $$ = zend_ast_create(ZEND_AST_VAR, $2); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '}'
			{ $$ = zend_ast_create(ZEND_AST_VAR, $2); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
			{ $$ = zend_ast_create(ZEND_AST_DIM,
			      zend_ast_create(ZEND_AST_VAR, $2), $4); }
	|	T_CURLY_OPEN variable '}' { $$ = $2; }
;

encaps_var_offset:
		T_STRING		{ $$ = $1; }
	|	T_NUM_STRING	{ $$ = $1; }
	|	T_VARIABLE		{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables ')' { $$ = $3; }
	|	T_EMPTY '(' expr ')' { $$ = zend_ast_create(ZEND_AST_EMPTY, $3); }
	|	T_INCLUDE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE, $2); }
	|	T_INCLUDE_ONCE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE_ONCE, $2); }
	|	T_EVAL '(' expr ')'
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_EVAL, $3); }
	|	T_REQUIRE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE, $2); }
	|	T_REQUIRE_ONCE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE_ONCE, $2); }
;

isset_variables:
		isset_variable { $$ = $1; }
	|	isset_variables ',' isset_variable
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
;

isset_variable:
		expr { $$ = zend_ast_create(ZEND_AST_ISSET, $1); }
;

class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS
		{ $$ = zend_ast_create(ZEND_AST_RESOLVE_CLASS_NAME, $1); }
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
			yystr_len = (unsigned int)yystrlen(yystr);

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
