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
%expect 3

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
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL
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

%% /* Rules */

start:
	top_statement_list	{ zend_do_end_compilation(TSRMLS_C); }
;

top_statement_list:
		top_statement_list  { zend_do_extended_info(TSRMLS_C); } top_statement { HANDLE_INTERACTIVE(); }
	|	/* empty */
;

namespace_name:
		T_STRING { $$ = $1; }
	|	namespace_name T_NS_SEPARATOR T_STRING { zend_do_build_namespace_name(&$$, &$1, &$3 TSRMLS_CC); }
;

top_statement:
		statement						{ zend_verify_namespace(TSRMLS_C); }
	|	function_declaration_statement	{ zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
	|	class_declaration_statement		{ zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
	|	T_HALT_COMPILER '(' ')' ';'		{ zend_do_halt_compiler_register(TSRMLS_C); YYACCEPT; }
	|	T_NAMESPACE namespace_name ';'	{ zend_do_begin_namespace(&$2, 0 TSRMLS_CC); }
	|	T_NAMESPACE namespace_name '{'	{ zend_do_begin_namespace(&$2, 1 TSRMLS_CC); }
		top_statement_list '}'		    { zend_do_end_namespace(TSRMLS_C); }
	|	T_NAMESPACE '{'					{ zend_do_begin_namespace(NULL, 1 TSRMLS_CC); }
		top_statement_list '}'			{ zend_do_end_namespace(TSRMLS_C); }
	|	T_USE use_declarations ';'      { zend_verify_namespace(TSRMLS_C); }
	|	T_USE T_FUNCTION use_function_declarations ';' { zend_verify_namespace(TSRMLS_C); }
	|	T_USE T_CONST use_const_declarations ';'       { zend_verify_namespace(TSRMLS_C); }
	|	constant_declaration ';'		{ zend_verify_namespace(TSRMLS_C); }
;

use_declarations:
		use_declarations ',' use_declaration
	|	use_declaration
;

use_declaration:
		namespace_name 			{ zend_do_use(&$1, NULL, 0 TSRMLS_CC); }
	|	namespace_name T_AS T_STRING	{ zend_do_use(&$1, &$3, 0 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { zend_do_use(&$2, NULL, 1 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING { zend_do_use(&$2, &$4, 1 TSRMLS_CC); }
;

use_function_declarations:
		use_function_declarations ',' use_function_declaration
	|	use_function_declaration
;

use_function_declaration:
		namespace_name 			{ zend_do_use_function(&$1, NULL, 0 TSRMLS_CC); }
	|	namespace_name T_AS T_STRING	{ zend_do_use_function(&$1, &$3, 0 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { zend_do_use_function(&$2, NULL, 1 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING { zend_do_use_function(&$2, &$4, 1 TSRMLS_CC); }
;

use_const_declarations:
		use_const_declarations ',' use_const_declaration
	|	use_const_declaration
;

use_const_declaration:
		namespace_name 			{ zend_do_use_const(&$1, NULL, 0 TSRMLS_CC); }
	|	namespace_name T_AS T_STRING	{ zend_do_use_const(&$1, &$3, 0 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { zend_do_use_const(&$2, NULL, 1 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name T_AS T_STRING { zend_do_use_const(&$2, &$4, 1 TSRMLS_CC); }
;

constant_declaration:
		constant_declaration ',' T_STRING '=' static_scalar	{ zend_do_declare_constant(&$3, &$5 TSRMLS_CC); }
	|	T_CONST T_STRING '=' static_scalar { zend_do_declare_constant(&$2, &$4 TSRMLS_CC); }
;

inner_statement_list:
		inner_statement_list  { zend_do_extended_info(TSRMLS_C); } inner_statement { HANDLE_INTERACTIVE(); }
	|	/* empty */
;


inner_statement:
		statement
	|	function_declaration_statement
	|	class_declaration_statement
	|	T_HALT_COMPILER '(' ')' ';'   { zend_error_noreturn(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
;


statement:
		unticked_statement { DO_TICKS(); }
	|	T_STRING ':' { zend_do_label(&$1 TSRMLS_CC); }
;

unticked_statement:
		'{' inner_statement_list '}'
	|	T_IF parenthesis_expr { zend_do_if_cond(&$2, &$1 TSRMLS_CC); } statement { zend_do_if_after_statement(&$1, 1 TSRMLS_CC); } elseif_list else_single { zend_do_if_end(TSRMLS_C); }
	|	T_IF parenthesis_expr ':' { zend_do_if_cond(&$2, &$1 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$1, 1 TSRMLS_CC); } new_elseif_list new_else_single T_ENDIF ';' { zend_do_if_end(TSRMLS_C); }
	|	T_WHILE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); } parenthesis_expr { zend_do_while_cond(&$3, &$$ TSRMLS_CC); } while_statement { zend_do_while_end(&$1, &$4 TSRMLS_CC); }
	|	T_DO { $1.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); } statement T_WHILE { $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); } parenthesis_expr ';' { zend_do_do_while_end(&$1, &$4, &$6 TSRMLS_CC); }
	|	T_FOR
			'('
				for_expr
			';' { zend_do_free(&$3 TSRMLS_CC); $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); }
				for_expr
			';' { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&$6, &$7 TSRMLS_CC); }
				for_expr
			')' { zend_do_free(&$9 TSRMLS_CC); zend_do_for_before_statement(&$4, &$7 TSRMLS_CC); }
			for_statement { zend_do_for_end(&$7 TSRMLS_CC); }
	|	T_SWITCH parenthesis_expr	{ zend_do_switch_cond(&$2 TSRMLS_CC); } switch_case_list { zend_do_switch_end(&$4 TSRMLS_CC); }
	|	T_BREAK ';'				{ zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
	|	T_BREAK expr ';'		{ zend_do_brk_cont(ZEND_BRK, &$2 TSRMLS_CC); }
	|	T_CONTINUE ';'			{ zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
	|	T_CONTINUE expr ';'		{ zend_do_brk_cont(ZEND_CONT, &$2 TSRMLS_CC); }
	|	T_RETURN ';'						{ zend_do_return(NULL, 0 TSRMLS_CC); }
	|	T_RETURN expr_without_variable ';'	{ zend_do_return(&$2, 0 TSRMLS_CC); }
	|	T_RETURN variable ';'				{ zend_do_return(&$2, 1 TSRMLS_CC); }
	|	yield_expr ';' { zend_do_free(&$1 TSRMLS_CC); }
	|	T_GLOBAL global_var_list ';'
	|	T_STATIC static_var_list ';'
	|	T_ECHO echo_expr_list ';'
	|	T_INLINE_HTML			{ zend_do_echo(&$1 TSRMLS_CC); }
	|	expr ';'				{ zend_do_free(&$1 TSRMLS_CC); }
	|	T_UNSET '(' unset_variables ')' ';'
	|	T_FOREACH '(' variable T_AS
		{ zend_do_foreach_begin(&$1, &$2, &$3, &$4, 1 TSRMLS_CC); }
		foreach_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
	|	T_FOREACH '(' expr_without_variable T_AS
		{ zend_do_foreach_begin(&$1, &$2, &$3, &$4, 0 TSRMLS_CC); }
		foreach_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
	|	T_DECLARE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); } '(' declare_list ')' declare_statement { zend_do_declare_end(&$1 TSRMLS_CC); }
	|	';'		/* empty statement */
	|	T_TRY { zend_do_try(&$1 TSRMLS_CC); } '{' inner_statement_list '}'
		catch_statement { zend_do_bind_catch(&$1, &$6 TSRMLS_CC); }
		finally_statement { zend_do_end_finally(&$1, &$6, &$8 TSRMLS_CC); }
	|	T_THROW expr ';' { zend_do_throw(&$2 TSRMLS_CC); }
	|	T_GOTO T_STRING ';' { zend_do_goto(&$2 TSRMLS_CC); }
;

catch_statement:
				/* empty */ { $$.op_type = IS_UNUSED; }
	|	T_CATCH '(' { zend_initialize_try_catch_element(&$1 TSRMLS_CC); } 
		fully_qualified_class_name { zend_do_first_catch(&$2 TSRMLS_CC); }
		T_VARIABLE ')' { zend_do_begin_catch(&$1, &$4, &$6, &$2 TSRMLS_CC); }
		'{' inner_statement_list '}' { zend_do_end_catch(&$1 TSRMLS_CC); }
		additional_catches { zend_do_mark_last_catch(&$2, &$13 TSRMLS_CC); $$ = $1;}

finally_statement:
					/* empty */ { $$.op_type = IS_UNUSED; }
	|	T_FINALLY { zend_do_finally(&$1 TSRMLS_CC); } '{' inner_statement_list '}' { $$ = $1; }
;

additional_catches:
		non_empty_additional_catches { $$ = $1; }
	|	/* empty */ { $$.u.op.opline_num = -1; }
;

non_empty_additional_catches:
		additional_catch { $$ = $1; }
	|	non_empty_additional_catches additional_catch { $$ = $2; }
;

additional_catch:
	T_CATCH '(' fully_qualified_class_name { $$.u.op.opline_num = get_next_op_number(CG(active_op_array)); } T_VARIABLE ')' { zend_do_begin_catch(&$1, &$3, &$5, NULL TSRMLS_CC); } '{' inner_statement_list '}' { zend_do_end_catch(&$1 TSRMLS_CC); }
;

unset_variables:
		unset_variable
	|	unset_variables ',' unset_variable
;

unset_variable:
		variable	{ zend_do_end_variable_parse(&$1, BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&$1 TSRMLS_CC); }
;

function_declaration_statement:
		unticked_function_declaration_statement	{ DO_TICKS(); }
;

class_declaration_statement:
		unticked_class_declaration_statement	{ DO_TICKS(); }
;

is_reference:
		/* empty */	{ $$.op_type = 0; }
	|	'&'			{ $$.op_type = 1; }
;

is_variadic:
		/* empty */ { $$.op_type = 0; }
	|	T_ELLIPSIS  { $$.op_type = 1; }
;

unticked_function_declaration_statement:
		function is_reference T_STRING { zend_do_begin_function_declaration(&$1, &$3, 0, $2.op_type, NULL TSRMLS_CC); }
		'(' parameter_list ')'
		'{' inner_statement_list '}' { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
;

unticked_class_declaration_statement:
		class_entry_type T_STRING extends_from
			{ zend_do_begin_class_declaration(&$1, &$2, &$3 TSRMLS_CC); }
			implements_list
			'{'
				class_statement_list
			'}' { zend_do_end_class_declaration(&$1, &$3 TSRMLS_CC); }
	|	interface_entry T_STRING
			{ zend_do_begin_class_declaration(&$1, &$2, NULL TSRMLS_CC); }
			interface_extends_list
			'{'
				class_statement_list
			'}' { zend_do_end_class_declaration(&$1, NULL TSRMLS_CC); }
;


class_entry_type:
		T_CLASS			{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = 0; }
	|	T_ABSTRACT T_CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
	|	T_TRAIT { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_TRAIT; }
	|	T_FINAL T_CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_FINAL_CLASS; }
;

extends_from:
		/* empty */					{ $$.op_type = IS_UNUSED; }
	|	T_EXTENDS fully_qualified_class_name	{ zend_do_fetch_class(&$$, &$2 TSRMLS_CC); }
;

interface_entry:
	T_INTERFACE		{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_INTERFACE; }
;

interface_extends_list:
		/* empty */
	|	T_EXTENDS interface_list
;

implements_list:
		/* empty */
	|	T_IMPLEMENTS interface_list
;

interface_list:
		fully_qualified_class_name			{ zend_do_implements_interface(&$1 TSRMLS_CC); }
	|	interface_list ',' fully_qualified_class_name { zend_do_implements_interface(&$3 TSRMLS_CC); }
;

foreach_optional_arg:
		/* empty */						{ $$.op_type = IS_UNUSED; }
	|	T_DOUBLE_ARROW foreach_variable	{ $$ = $2; }
;

foreach_variable:
		variable			{ zend_check_writable_variable(&$1); $$ = $1; }
	|	'&' variable		{ zend_check_writable_variable(&$2); $$ = $2;  $$.EA |= ZEND_PARSED_REFERENCE_VARIABLE; }
	|	T_LIST '(' { zend_do_list_init(TSRMLS_C); } assignment_list ')' { $$ = $1; $$.EA = ZEND_PARSED_LIST_EXPR; }
;

for_statement:
		statement
	|	':' inner_statement_list T_ENDFOR ';'
;


foreach_statement:
		statement
	|	':' inner_statement_list T_ENDFOREACH ';'
;


declare_statement:
		statement
	|	':' inner_statement_list T_ENDDECLARE ';'
;


declare_list:
		T_STRING '=' static_scalar					{ zend_do_declare_stmt(&$1, &$3 TSRMLS_CC); }
	|	declare_list ',' T_STRING '=' static_scalar	{ zend_do_declare_stmt(&$3, &$5 TSRMLS_CC); }
;


switch_case_list:
		'{' case_list '}'					{ $$ = $2; }
	|	'{' ';' case_list '}'				{ $$ = $3; }
	|	':' case_list T_ENDSWITCH ';'		{ $$ = $2; }
	|	':' ';' case_list T_ENDSWITCH ';'	{ $$ = $3; }
;


case_list:
		/* empty */	{ $$.op_type = IS_UNUSED; }
	|	case_list T_CASE expr case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&$1, &$2, &$3 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
	|	case_list T_DEFAULT case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_default_before_statement(&$1, &$2 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
;


case_separator:
		':'
	|	';'
;


while_statement:
		statement
	|	':' inner_statement_list T_ENDWHILE ';'
;



elseif_list:
		/* empty */
	|	elseif_list T_ELSEIF parenthesis_expr { zend_do_if_cond(&$3, &$2 TSRMLS_CC); } statement { zend_do_if_after_statement(&$2, 0 TSRMLS_CC); }
;


new_elseif_list:
		/* empty */
	|	new_elseif_list T_ELSEIF parenthesis_expr ':' { zend_do_if_cond(&$3, &$2 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$2, 0 TSRMLS_CC); }
;


else_single:
		/* empty */
	|	T_ELSE statement
;


new_else_single:
		/* empty */
	|	T_ELSE ':' inner_statement_list
;


parameter_list:
		non_empty_parameter_list
	|	/* empty */
;


non_empty_parameter_list:
		parameter
	|	non_empty_parameter_list ',' parameter
;

parameter:
		optional_class_type is_reference is_variadic T_VARIABLE
			{ zend_do_receive_param(ZEND_RECV, &$4, NULL, &$1, $2.op_type, $3.op_type TSRMLS_CC); }
	|	optional_class_type is_reference is_variadic T_VARIABLE '=' static_scalar
			{ zend_do_receive_param(ZEND_RECV_INIT, &$4, &$6, &$1, $2.op_type, $3.op_type TSRMLS_CC); }
;


optional_class_type:
		/* empty */					{ $$.op_type = IS_UNUSED; }
	|	T_ARRAY						{ $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_ARRAY; }
	|	T_CALLABLE					{ $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_CALLABLE; }
	|	fully_qualified_class_name			{ $$ = $1; }
;


function_call_parameter_list:
		'(' ')'	{ Z_LVAL($$.u.constant) = 0; }
	|	'(' non_empty_function_call_parameter_list ')'	{ $$ = $2; }
	|	'(' yield_expr ')'	{ Z_LVAL($$.u.constant) = 1; zend_do_pass_param(&$2, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
;


non_empty_function_call_parameter_list:
		expr_without_variable	{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
	|	variable				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
	|	'&' w_variable 				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$2, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' expr_without_variable	{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' variable					{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' '&' w_variable			{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$4, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
;

global_var_list:
		global_var_list ',' global_var	{ zend_do_fetch_global_variable(&$3, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
	|	global_var						{ zend_do_fetch_global_variable(&$1, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
;


global_var:
		T_VARIABLE			{ $$ = $1; }
	|	'$' r_variable		{ $$ = $2; }
	|	'$' '{' expr '}'	{ $$ = $3; }
;


static_var_list:
		static_var_list ',' T_VARIABLE { zend_do_fetch_static_variable(&$3, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	static_var_list ',' T_VARIABLE '=' static_scalar { zend_do_fetch_static_variable(&$3, &$5, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE  { zend_do_fetch_static_variable(&$1, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE '=' static_scalar { zend_do_fetch_static_variable(&$1, &$3, ZEND_FETCH_STATIC TSRMLS_CC); }

;


class_statement_list:
		class_statement_list class_statement
	|	/* empty */
;


class_statement:
		variable_modifiers { CG(access_type) = Z_LVAL($1.u.constant); } class_variable_declaration ';'
	|	class_constant_declaration ';'
	|	trait_use_statement
	|	method_modifiers function is_reference T_STRING { zend_do_begin_function_declaration(&$2, &$4, 1, $3.op_type, &$1 TSRMLS_CC); }
		'(' parameter_list ')'
		method_body { zend_do_abstract_method(&$4, &$1, &$9 TSRMLS_CC); zend_do_end_function_declaration(&$2 TSRMLS_CC); }
;

trait_use_statement:
		T_USE trait_list trait_adaptations
;

trait_list:
		fully_qualified_class_name						{ zend_do_use_trait(&$1 TSRMLS_CC); }
	|	trait_list ',' fully_qualified_class_name		{ zend_do_use_trait(&$3 TSRMLS_CC); }
;

trait_adaptations:
		';'
	|	'{' trait_adaptation_list '}'
;

trait_adaptation_list:
		/* empty */
	|	non_empty_trait_adaptation_list
;

non_empty_trait_adaptation_list:
		trait_adaptation_statement
	|	non_empty_trait_adaptation_list trait_adaptation_statement
;

trait_adaptation_statement:
		trait_precedence ';'
	|	trait_alias ';'
;

trait_precedence:
	trait_method_reference_fully_qualified T_INSTEADOF trait_reference_list	{ zend_add_trait_precedence(&$1, &$3 TSRMLS_CC); }
;

trait_reference_list:
		fully_qualified_class_name									{ zend_resolve_class_name(&$1 TSRMLS_CC); zend_init_list(&$$.u.op.ptr, Z_STRVAL($1.u.constant) TSRMLS_CC); }
	|	trait_reference_list ',' fully_qualified_class_name			{ zend_resolve_class_name(&$3 TSRMLS_CC); zend_add_to_list(&$1.u.op.ptr, Z_STRVAL($3.u.constant) TSRMLS_CC); $$ = $1; }
;

trait_method_reference:
		T_STRING													{ zend_prepare_reference(&$$, NULL, &$1 TSRMLS_CC); }
	|	trait_method_reference_fully_qualified						{ $$ = $1; }
;

trait_method_reference_fully_qualified:
	fully_qualified_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING		{ zend_prepare_reference(&$$, &$1, &$3 TSRMLS_CC); }
;

trait_alias:
		trait_method_reference T_AS trait_modifiers T_STRING		{ zend_add_trait_alias(&$1, &$3, &$4 TSRMLS_CC); }
	|	trait_method_reference T_AS member_modifier					{ zend_add_trait_alias(&$1, &$3, NULL TSRMLS_CC); }
;

trait_modifiers:
		/* empty */					{ Z_LVAL($$.u.constant) = 0x0; } /* No change of methods visibility */
	|	member_modifier	{ $$ = $1; } /* REM: Keep in mind, there are not only visibility modifiers */
;

method_body:
		';' /* abstract method */		{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
	|	'{' inner_statement_list '}'	{ Z_LVAL($$.u.constant) = 0;	}
;

variable_modifiers:
		non_empty_member_modifiers		{ $$ = $1; }
	|	T_VAR							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
;

method_modifiers:
		/* empty */							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers			{ $$ = $1;  if (!(Z_LVAL($$.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL($$.u.constant) |= ZEND_ACC_PUBLIC; } }
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

class_variable_declaration:
		class_variable_declaration ',' T_VARIABLE					{ zend_do_declare_property(&$3, NULL, CG(access_type) TSRMLS_CC); }
	|	class_variable_declaration ',' T_VARIABLE '=' static_scalar	{ zend_do_declare_property(&$3, &$5, CG(access_type) TSRMLS_CC); }
	|	T_VARIABLE						{ zend_do_declare_property(&$1, NULL, CG(access_type) TSRMLS_CC); }
	|	T_VARIABLE '=' static_scalar	{ zend_do_declare_property(&$1, &$3, CG(access_type) TSRMLS_CC); }
;

class_constant_declaration:
		class_constant_declaration ',' T_STRING '=' static_scalar	{ zend_do_declare_class_constant(&$3, &$5 TSRMLS_CC); }
	|	T_CONST T_STRING '=' static_scalar	{ zend_do_declare_class_constant(&$2, &$4 TSRMLS_CC); }
;

echo_expr_list:
		echo_expr_list ',' expr { zend_do_echo(&$3 TSRMLS_CC); }
	|	expr					{ zend_do_echo(&$1 TSRMLS_CC); }
;


for_expr:
		/* empty */			{ $$.op_type = IS_CONST;  Z_TYPE($$.u.constant) = IS_BOOL;  Z_LVAL($$.u.constant) = 1; }
	|	non_empty_for_expr	{ $$ = $1; }
;

non_empty_for_expr:
		non_empty_for_expr ','	{ zend_do_free(&$1 TSRMLS_CC); } expr { $$ = $4; }
	|	expr					{ $$ = $1; }
;

chaining_method_or_property:
		chaining_method_or_property variable_property 	{ $$.EA = $2.EA; }
	|	variable_property 								{ $$.EA = $1.EA; }
;

chaining_dereference:
		chaining_dereference '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	|	'[' dim_offset ']'		{ zend_do_pop_object(&$1 TSRMLS_CC); fetch_array_dim(&$$, &$1, &$2 TSRMLS_CC); }
;

chaining_instance_call:
		chaining_dereference 		{ zend_do_push_object(&$1 TSRMLS_CC); } chaining_method_or_property { $$ = $3; }
	|	chaining_dereference 		{ zend_do_push_object(&$1 TSRMLS_CC); $$ = $1; }
	|	chaining_method_or_property { $$ = $1; }
;

instance_call:
		/* empty */ 		{ $$ = $0; }
	|	{ zend_do_push_object(&$0 TSRMLS_CC); zend_do_begin_variable_parse(TSRMLS_C); }
		chaining_instance_call	{ zend_do_pop_object(&$$ TSRMLS_CC); zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC); }
;

new_expr:
		T_NEW class_name_reference { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$1, &$2 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$$, &$1, &$4 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
;

expr_without_variable:
		T_LIST '(' { zend_do_list_init(TSRMLS_C); } assignment_list ')' '=' expr { zend_do_list_end(&$$, &$7 TSRMLS_CC); }
	|	variable '=' expr		{ zend_check_writable_variable(&$1); zend_do_assign(&$$, &$1, &$3 TSRMLS_CC); }
	|	variable '=' '&' variable { zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$4, BP_VAR_W, 1 TSRMLS_CC); zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&$$, &$1, &$4 TSRMLS_CC); }
	|	variable '=' '&' T_NEW class_name_reference { zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");  zend_check_writable_variable(&$1); zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$4, &$5 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$3, &$4, &$7 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $3.EA = ZEND_PARSED_NEW; zend_do_assign_ref(&$$, &$1, &$3 TSRMLS_CC); }
	|	T_CLONE expr { zend_do_clone(&$$, &$2 TSRMLS_CC); }
	|	variable T_PLUS_EQUAL expr 	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_ADD, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MINUS_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SUB, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MUL_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MUL, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_DIV_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_DIV, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_CONCAT_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_CONCAT, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MOD_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MOD, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_AND_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_AND, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_OR_EQUAL expr 		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_OR, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_XOR_EQUAL expr 		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_SL_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SL, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_SR_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SR, &$$, &$1, &$3 TSRMLS_CC); }
	|	rw_variable T_INC { zend_do_post_incdec(&$$, &$1, ZEND_POST_INC TSRMLS_CC); }
	|	T_INC rw_variable { zend_do_pre_incdec(&$$, &$2, ZEND_PRE_INC TSRMLS_CC); }
	|	rw_variable T_DEC { zend_do_post_incdec(&$$, &$1, ZEND_POST_DEC TSRMLS_CC); }
	|	T_DEC rw_variable { zend_do_pre_incdec(&$$, &$2, ZEND_PRE_DEC TSRMLS_CC); }
	|	expr T_BOOLEAN_OR { zend_do_boolean_or_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_or_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
	|	expr T_BOOLEAN_AND { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_and_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
	|	expr T_LOGICAL_OR { zend_do_boolean_or_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_or_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
	|	expr T_LOGICAL_AND { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_and_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
	|	expr T_LOGICAL_XOR expr { zend_do_binary_op(ZEND_BOOL_XOR, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '|' expr	{ zend_do_binary_op(ZEND_BW_OR, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '&' expr	{ zend_do_binary_op(ZEND_BW_AND, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '^' expr	{ zend_do_binary_op(ZEND_BW_XOR, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '.' expr 	{ zend_do_binary_op(ZEND_CONCAT, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '+' expr 	{ zend_do_binary_op(ZEND_ADD, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '-' expr 	{ zend_do_binary_op(ZEND_SUB, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '*' expr	{ zend_do_binary_op(ZEND_MUL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '/' expr	{ zend_do_binary_op(ZEND_DIV, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '%' expr 	{ zend_do_binary_op(ZEND_MOD, &$$, &$1, &$3 TSRMLS_CC); }
	| 	expr T_SL expr	{ zend_do_binary_op(ZEND_SL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr T_SR expr	{ zend_do_binary_op(ZEND_SR, &$$, &$1, &$3 TSRMLS_CC); }
	|	'+' expr %prec T_INC { ZVAL_LONG(&$1.u.constant, 0); if ($2.op_type == IS_CONST) { add_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; } else { $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_ADD, &$$, &$1, &$2 TSRMLS_CC); } }
	|	'-' expr %prec T_INC { ZVAL_LONG(&$1.u.constant, 0); if ($2.op_type == IS_CONST) { sub_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; } else { $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_SUB, &$$, &$1, &$2 TSRMLS_CC); } }
	|	'!' expr { zend_do_unary_op(ZEND_BOOL_NOT, &$$, &$2 TSRMLS_CC); }
	|	'~' expr { zend_do_unary_op(ZEND_BW_NOT, &$$, &$2 TSRMLS_CC); }
	|	expr T_IS_IDENTICAL expr		{ zend_do_binary_op(ZEND_IS_IDENTICAL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr T_IS_NOT_IDENTICAL expr	{ zend_do_binary_op(ZEND_IS_NOT_IDENTICAL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr T_IS_EQUAL expr			{ zend_do_binary_op(ZEND_IS_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr T_IS_NOT_EQUAL expr 		{ zend_do_binary_op(ZEND_IS_NOT_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '<' expr 					{ zend_do_binary_op(ZEND_IS_SMALLER, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr T_IS_SMALLER_OR_EQUAL expr { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
	|	expr '>' expr 					{ zend_do_binary_op(ZEND_IS_SMALLER, &$$, &$3, &$1 TSRMLS_CC); }
	|	expr T_IS_GREATER_OR_EQUAL expr { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1 TSRMLS_CC); }
	|	expr T_INSTANCEOF class_name_reference { zend_do_instanceof(&$$, &$1, &$3, 0 TSRMLS_CC); }
	|	parenthesis_expr 	{ $$ = $1; }
	|	new_expr		{ $$ = $1; }
	|	'(' new_expr ')' { $$ = $2; } instance_call { $$ = $5; }
	|	expr '?' { zend_do_begin_qm_op(&$1, &$2 TSRMLS_CC); }
		expr ':' { zend_do_qm_true(&$4, &$2, &$5 TSRMLS_CC); }
		expr	 { zend_do_qm_false(&$$, &$7, &$2, &$5 TSRMLS_CC); }
	|	expr '?' ':' { zend_do_jmp_set(&$1, &$2, &$3 TSRMLS_CC); }
		expr     { zend_do_jmp_set_else(&$$, &$5, &$2, &$3 TSRMLS_CC); }
	|	internal_functions_in_yacc { $$ = $1; }
	|	T_INT_CAST expr 	{ zend_do_cast(&$$, &$2, IS_LONG TSRMLS_CC); }
	|	T_DOUBLE_CAST expr 	{ zend_do_cast(&$$, &$2, IS_DOUBLE TSRMLS_CC); }
	|	T_STRING_CAST expr	{ zend_do_cast(&$$, &$2, IS_STRING TSRMLS_CC); }
	|	T_ARRAY_CAST expr 	{ zend_do_cast(&$$, &$2, IS_ARRAY TSRMLS_CC); }
	|	T_OBJECT_CAST expr 	{ zend_do_cast(&$$, &$2, IS_OBJECT TSRMLS_CC); }
	|	T_BOOL_CAST expr	{ zend_do_cast(&$$, &$2, IS_BOOL TSRMLS_CC); }
	|	T_UNSET_CAST expr	{ zend_do_cast(&$$, &$2, IS_NULL TSRMLS_CC); }
	|	T_EXIT exit_expr	{ zend_do_exit(&$$, &$2 TSRMLS_CC); }
	|	'@' { zend_do_begin_silence(&$1 TSRMLS_CC); } expr { zend_do_end_silence(&$1 TSRMLS_CC); $$ = $3; }
	|	scalar				{ $$ = $1; }
	|	combined_scalar_offset { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); }
	|	combined_scalar { $$ = $1; }
	|	'`' backticks_expr '`' { zend_do_shell_exec(&$$, &$2 TSRMLS_CC); }
	|	T_PRINT expr  { zend_do_print(&$$, &$2 TSRMLS_CC); }
	|	T_YIELD { zend_do_yield(&$$, NULL, NULL, 0 TSRMLS_CC); }
	|	function is_reference { zend_do_begin_lambda_function_declaration(&$$, &$1, $2.op_type, 0 TSRMLS_CC); }
		'(' parameter_list ')' lexical_vars
		'{' inner_statement_list '}' { zend_do_end_function_declaration(&$1 TSRMLS_CC); $$ = $3; }
	|	T_STATIC function is_reference { zend_do_begin_lambda_function_declaration(&$$, &$2, $3.op_type, 1 TSRMLS_CC); }
		'(' parameter_list ')' lexical_vars
		'{' inner_statement_list '}' { zend_do_end_function_declaration(&$2 TSRMLS_CC); $$ = $4; }
;

yield_expr:
		T_YIELD expr_without_variable { zend_do_yield(&$$, &$2, NULL, 0 TSRMLS_CC); }
	|	T_YIELD variable { zend_do_yield(&$$, &$2, NULL, 1 TSRMLS_CC); }
	|	T_YIELD expr T_DOUBLE_ARROW expr_without_variable { zend_do_yield(&$$, &$4, &$2, 0 TSRMLS_CC); }
	|	T_YIELD expr T_DOUBLE_ARROW variable { zend_do_yield(&$$, &$4, &$2, 1 TSRMLS_CC); }
;

combined_scalar_offset:
	  combined_scalar '[' dim_offset ']' { zend_do_begin_variable_parse(TSRMLS_C); fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	| combined_scalar_offset '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
    | T_CONSTANT_ENCAPSED_STRING '[' dim_offset ']' { $1.EA = 0; zend_do_begin_variable_parse(TSRMLS_C); fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }

combined_scalar:
      T_ARRAY '(' array_pair_list ')' { $$ = $3; }
    | '[' array_pair_list ']' { $$ = $2; }

function:
	T_FUNCTION { $$.u.op.opline_num = CG(zend_lineno); }
;

lexical_vars:
		/* empty */
	|	T_USE '(' lexical_var_list ')'
;

lexical_var_list:
		lexical_var_list ',' T_VARIABLE			{ zend_do_fetch_lexical_variable(&$3, 0 TSRMLS_CC); }
	|	lexical_var_list ',' '&' T_VARIABLE		{ zend_do_fetch_lexical_variable(&$4, 1 TSRMLS_CC); }
	|	T_VARIABLE								{ zend_do_fetch_lexical_variable(&$1, 0 TSRMLS_CC); }
	|	'&' T_VARIABLE							{ zend_do_fetch_lexical_variable(&$2, 1 TSRMLS_CC); }
;

function_call:
		namespace_name { $$.u.op.opline_num = zend_do_begin_function_call(&$1, 1 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(&$1, &$$, &$3, 0, $2.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $1.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$1.u.constant);  zend_do_build_namespace_name(&$1, &$1, &$3 TSRMLS_CC); $$.u.op.opline_num = zend_do_begin_function_call(&$1, 0 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(&$1, &$$, &$5, 0, $4.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
	|	T_NS_SEPARATOR namespace_name { $$.u.op.opline_num = zend_do_begin_function_call(&$2, 0 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(&$2, &$$, &$4, 0, $3.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM variable_name { $$.u.op.opline_num = zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call($4.u.op.opline_num?NULL:&$3, &$$, &$5, $4.u.op.opline_num, $4.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects { zend_do_end_variable_parse(&$3, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(NULL, &$$, &$5, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_name { zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(NULL, &$$, &$5, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects { zend_do_end_variable_parse(&$3, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(NULL, &$$, &$5, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	variable_without_objects { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_dynamic_function_call(&$1, 0 TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(&$1, &$$, &$3, 0, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
;

class_name:
		T_STATIC { $$.op_type = IS_CONST; ZVAL_STRINGL(&$$.u.constant, "static", sizeof("static")-1, 1);}
	|	namespace_name { $$ = $1; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
;

fully_qualified_class_name:
		namespace_name { $$ = $1; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
;



class_name_reference:
		class_name						{ zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
	|	dynamic_class_name_reference	{ zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
;


dynamic_class_name_reference:
		base_variable T_OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
			object_property { zend_do_push_object(&$4 TSRMLS_CC); } dynamic_class_name_variable_properties
			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = ZEND_PARSED_MEMBER; }
	|	base_variable { $$ = $1; }
;


dynamic_class_name_variable_properties:
		dynamic_class_name_variable_properties dynamic_class_name_variable_property
	|	/* empty */
;


dynamic_class_name_variable_property:
		T_OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); }
;

exit_expr:
		/* empty */	{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
	|	'(' ')'		{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
	|	parenthesis_expr	{ $$ = $1; }
;

backticks_expr:
		/* empty */	{ ZVAL_EMPTY_STRING(&$$.u.constant); INIT_PZVAL(&$$.u.constant); $$.op_type = IS_CONST; }
	|	T_ENCAPSED_AND_WHITESPACE	{ $$ = $1; }
	|	encaps_list	{ $$ = $1; }
;


ctor_arguments:
		/* empty */	{ Z_LVAL($$.u.constant) = 0; }
	|	function_call_parameter_list 	{ $$ = $1; }
;


common_scalar:
		T_LNUMBER 					{ $$ = $1; }
	|	T_DNUMBER 					{ $$ = $1; }
	|	T_CONSTANT_ENCAPSED_STRING	{ $$ = $1; }
	|	T_LINE 						{ $$ = $1; }
	|	T_FILE 						{ $$ = $1; }
	|	T_DIR   					{ $$ = $1; }
	|	T_TRAIT_C					{ $$ = $1; }
	|	T_METHOD_C					{ $$ = $1; }
	|	T_FUNC_C					{ $$ = $1; }
	|	T_NS_C						{ $$ = $1; }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$ = $2; }
	|	T_START_HEREDOC T_END_HEREDOC { ZVAL_EMPTY_STRING(&$$.u.constant); INIT_PZVAL(&$$.u.constant); $$.op_type = IS_CONST; }
;

static_class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_CT, 0 TSRMLS_CC); }
;

static_scalar: /* compile-time evaluated scalars */
		static_scalar_value { zend_do_constant_expression(&$$, $1.u.ast TSRMLS_CC); }
	|	T_ARRAY '(' static_array_pair_list ')' { $$ = $3; Z_TYPE($$.u.constant) = IS_CONSTANT_ARRAY; }
	|	'[' static_array_pair_list ']' { $$ = $2; Z_TYPE($$.u.constant) = IS_CONSTANT_ARRAY; }
;

static_scalar_value:
		common_scalar	{ $$.u.ast = zend_ast_create_constant(&$1.u.constant); }
	|	static_class_name_scalar	{ $$.u.ast = zend_ast_create_constant(&$1.u.constant); }
	|	namespace_name 		{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_CT, 1 TSRMLS_CC); $$.u.ast = zend_ast_create_constant(&$$.u.constant); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_CT, 0 TSRMLS_CC); $$.u.ast = zend_ast_create_constant(&$$.u.constant); }
	|	T_NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); zend_do_fetch_constant(&$$, NULL, &$2, ZEND_CT, 0 TSRMLS_CC); $$.u.ast = zend_ast_create_constant(&$$.u.constant); }
	|	static_class_constant { $$.u.ast = zend_ast_create_constant(&$1.u.constant); }
	|	T_CLASS_C			{ $$.u.ast = zend_ast_create_constant(&$1.u.constant); }
	|	static_operation { $$ = $1; }
;

static_operation:
		static_scalar_value '+' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_ADD, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '-' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_SUB, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '*' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_MUL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '/' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_DIV, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '%' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_MOD, $1.u.ast, $3.u.ast); }
	|	'!' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_BOOL_NOT, $2.u.ast); }
	|	'~' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_BW_NOT, $2.u.ast); }
	|	static_scalar_value '|' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BW_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '&' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BW_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '^' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BW_XOR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_SL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_SL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_SR static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_SR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '.' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_CONCAT, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_LOGICAL_XOR static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BOOL_XOR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_LOGICAL_AND static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BOOL_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_LOGICAL_OR static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BOOL_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_BOOLEAN_AND static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BOOL_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_BOOLEAN_OR static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_BOOL_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_IDENTICAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_NOT_IDENTICAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_NOT_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_EQUAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_NOT_EQUAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_NOT_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '<' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_SMALLER, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '>' static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_SMALLER, $3.u.ast, $1.u.ast); }
	|	static_scalar_value T_IS_SMALLER_OR_EQUAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_SMALLER_OR_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_GREATER_OR_EQUAL static_scalar_value { $$.u.ast = zend_ast_create_binary(ZEND_IS_SMALLER_OR_EQUAL, $3.u.ast, $1.u.ast); }
	|	static_scalar_value '?' ':' static_scalar_value { $$.u.ast = zend_ast_create_ternary(ZEND_SELECT, $1.u.ast, NULL, $4.u.ast); }
	|	static_scalar_value '?' static_scalar_value ':' static_scalar_value { $$.u.ast = zend_ast_create_ternary(ZEND_SELECT, $1.u.ast, $3.u.ast, $5.u.ast); }
	|	'+' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_UNARY_PLUS, $2.u.ast); }
	|	'-' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_UNARY_MINUS, $2.u.ast); }
	|	'(' static_scalar_value ')' { $$ = $2; }
;


scalar:
		T_STRING_VARNAME		{ $$ = $1; }
	|	class_name_scalar	{ $$ = $1; }
	|	class_constant		{ $$ = $1; }
	|	namespace_name	{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_RT, 1 TSRMLS_CC); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_RT, 0 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); zend_do_fetch_constant(&$$, NULL, &$2, ZEND_RT, 0 TSRMLS_CC); }
	|	common_scalar			{ $$ = $1; }
	|	'"' encaps_list '"' 	{ $$ = $2; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$ = $2; }
	|	T_CLASS_C				{ if (Z_TYPE($1.u.constant) == IS_CONSTANT) {zend_do_fetch_constant(&$$, NULL, &$1, ZEND_RT, 1 TSRMLS_CC);} else {$$ = $1;} }
;


static_array_pair_list:
		/* empty */ { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); }
	|	non_empty_static_array_pair_list possible_comma	{ $$ = $1; }
;

possible_comma:
		/* empty */
	|	','
;

non_empty_static_array_pair_list:
		non_empty_static_array_pair_list ',' static_scalar T_DOUBLE_ARROW static_scalar	{ zend_do_add_static_array_element(&$$, &$3, &$5); }
	|	non_empty_static_array_pair_list ',' static_scalar { zend_do_add_static_array_element(&$$, NULL, &$3); }
	|	static_scalar T_DOUBLE_ARROW static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, &$1, &$3); }
	|	static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, NULL, &$1); }
;

expr:
		r_variable					{ $$ = $1; }
	|	expr_without_variable		{ $$ = $1; }
;

parenthesis_expr:
		'(' expr ')'		{ $$ = $2; }
	|	'(' yield_expr ')'	{ $$ = $2; }
;


r_variable:
	variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$ = $1; }
;


w_variable:
	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $$ = $1;
				  zend_check_writable_variable(&$1); }
;

rw_variable:
	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); $$ = $1;
				  zend_check_writable_variable(&$1); }
;

variable:
		base_variable_with_function_calls T_OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
			object_property { zend_do_push_object(&$4 TSRMLS_CC); } method_or_not variable_properties
			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = $1.EA | ($7.EA ? $7.EA : $6.EA); }
	|	base_variable_with_function_calls { $$ = $1; }
;

variable_properties:
		variable_properties variable_property { $$.EA = $2.EA; }
	|	/* empty */ { $$.EA = 0; }
;


variable_property:
		T_OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); } method_or_not { $$.EA = $4.EA; }
;

array_method_dereference:
		array_method_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	|	method '[' dim_offset ']' { $1.EA = ZEND_PARSED_METHOD_CALL; fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
;

method:
		{ zend_do_pop_object(&$$ TSRMLS_CC); zend_do_begin_method_call(&$$ TSRMLS_CC); }
		function_call_parameter_list { zend_do_end_function_call(&$1, &$$, &$2, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
;

method_or_not:
		method						{ $$ = $1; $$.EA = ZEND_PARSED_METHOD_CALL; zend_do_push_object(&$$ TSRMLS_CC); }
	|	array_method_dereference	{ $$ = $1; zend_do_push_object(&$$ TSRMLS_CC); }
	|	/* empty */ { $$.EA = ZEND_PARSED_MEMBER; }
;

variable_without_objects:
		reference_variable { $$ = $1; }
	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); }
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }

;

variable_class_name:
		reference_variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$=$1;; }
;

array_function_dereference:
		array_function_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $1.EA = ZEND_PARSED_FUNCTION_CALL; }
		'[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$4 TSRMLS_CC); }
;

base_variable_with_function_calls:
		base_variable				{ $$ = $1; }
	|	array_function_dereference	{ $$ = $1; }
	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$ = $1; $$.EA = ZEND_PARSED_FUNCTION_CALL; }
;


base_variable:
		reference_variable { $$ = $1; $$.EA = ZEND_PARSED_VARIABLE; }
	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); $$.EA = ZEND_PARSED_VARIABLE; }
	|	static_member { $$ = $1; $$.EA = ZEND_PARSED_STATIC_MEMBER; }
;

reference_variable:
		reference_variable '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	|	reference_variable '{' expr '}'		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
	|	compound_variable			{ zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
;


compound_variable:
		T_VARIABLE			{ $$ = $1; }
	|	'$' '{' expr '}'	{ $$ = $3; }
;

dim_offset:
		/* empty */		{ $$.op_type = IS_UNUSED; }
	|	expr			{ $$ = $1; }
;


object_property:
		object_dim_list { $$ = $1; }
	|	variable_without_objects { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); } { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
;

object_dim_list:
		object_dim_list '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
	|	object_dim_list '{' expr '}'		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
	|	variable_name { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
;

variable_name:
		T_STRING		{ $$ = $1; }
	|	'{' expr '}'	{ $$ = $2; }
;

simple_indirect_reference:
		'$' { Z_LVAL($$.u.constant) = 1; }
	|	simple_indirect_reference '$' { Z_LVAL($$.u.constant)++; }
;

assignment_list:
		assignment_list ',' assignment_list_element
	|	assignment_list_element
;


assignment_list_element:
		variable								{ zend_do_add_list_element(&$1 TSRMLS_CC); }
	|	T_LIST '(' { zend_do_new_list_begin(TSRMLS_C); } assignment_list ')'	{ zend_do_new_list_end(TSRMLS_C); }
	|	/* empty */							{ zend_do_add_list_element(NULL TSRMLS_CC); }
;


array_pair_list:
		/* empty */ { zend_do_init_array(&$$, NULL, NULL, 0 TSRMLS_CC); }
	|	non_empty_array_pair_list possible_comma	{ $$ = $1; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' expr T_DOUBLE_ARROW expr	{ zend_do_add_array_element(&$$, &$5, &$3, 0 TSRMLS_CC); }
	|	non_empty_array_pair_list ',' expr			{ zend_do_add_array_element(&$$, &$3, NULL, 0 TSRMLS_CC); }
	|	expr T_DOUBLE_ARROW expr	{ zend_do_init_array(&$$, &$3, &$1, 0 TSRMLS_CC); }
	|	expr 				{ zend_do_init_array(&$$, &$1, NULL, 0 TSRMLS_CC); }
	|	non_empty_array_pair_list ',' expr T_DOUBLE_ARROW '&' w_variable { zend_do_add_array_element(&$$, &$6, &$3, 1 TSRMLS_CC); }
	|	non_empty_array_pair_list ',' '&' w_variable { zend_do_add_array_element(&$$, &$4, NULL, 1 TSRMLS_CC); }
	|	expr T_DOUBLE_ARROW '&' w_variable	{ zend_do_init_array(&$$, &$4, &$1, 1 TSRMLS_CC); }
	|	'&' w_variable 			{ zend_do_init_array(&$$, &$2, NULL, 1 TSRMLS_CC); }
;

encaps_list:
		encaps_list encaps_var { zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE	{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_var { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, NULL, &$1 TSRMLS_CC); }
	|	T_ENCAPSED_AND_WHITESPACE encaps_var	{ zend_do_add_string(&$$, NULL, &$1 TSRMLS_CC); zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, &$$, &$2 TSRMLS_CC); }
;



encaps_var:
		T_VARIABLE { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
	|	T_VARIABLE '[' { zend_do_begin_variable_parse(TSRMLS_C); } encaps_var_offset ']'	{ fetch_array_begin(&$$, &$1, &$4 TSRMLS_CC); }
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$2, &$1, 1 TSRMLS_CC); zend_do_fetch_property(&$$, &$2, &$3 TSRMLS_CC); }
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}' { zend_do_begin_variable_parse(TSRMLS_C);  fetch_simple_variable(&$$, &$2, 1 TSRMLS_CC); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}' { zend_do_begin_variable_parse(TSRMLS_C);  fetch_array_begin(&$$, &$2, &$4 TSRMLS_CC); }
	|	T_CURLY_OPEN variable '}' { $$ = $2; }
;


encaps_var_offset:
		T_STRING		{ $$ = $1; }
	|	T_NUM_STRING	{ $$ = $1; }
	|	T_VARIABLE		{ fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables ')' { $$ = $3; }
	|	T_EMPTY '(' variable ')'	{ zend_do_isset_or_isempty(ZEND_ISEMPTY, &$$, &$3 TSRMLS_CC); }
	|	T_EMPTY '(' expr_without_variable ')' { zend_do_unary_op(ZEND_BOOL_NOT, &$$, &$3 TSRMLS_CC); }
	|	T_INCLUDE expr 			{ zend_do_include_or_eval(ZEND_INCLUDE, &$$, &$2 TSRMLS_CC); }
	|	T_INCLUDE_ONCE expr 	{ zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &$$, &$2 TSRMLS_CC); }
	|	T_EVAL '(' expr ')' 	{ zend_do_include_or_eval(ZEND_EVAL, &$$, &$3 TSRMLS_CC); }
	|	T_REQUIRE expr			{ zend_do_include_or_eval(ZEND_REQUIRE, &$$, &$2 TSRMLS_CC); }
	|	T_REQUIRE_ONCE expr		{ zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &$$, &$2 TSRMLS_CC); }
;

isset_variables:
		isset_variable			{ $$ = $1; }
	|	isset_variables ',' { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } isset_variable { zend_do_boolean_and_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
;

isset_variable:
		variable				{ zend_do_isset_or_isempty(ZEND_ISSET, &$$, &$1 TSRMLS_CC); }
	|	expr_without_variable	{ zend_error_noreturn(E_COMPILE_ERROR, "Cannot use isset() on the result of an expression (you can use \"null !== expression\" instead)"); }
;

class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
;

static_class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS { zend_do_resolve_class_name(&$$, &$1, 1 TSRMLS_CC); }
;

class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS { zend_do_resolve_class_name(&$$, &$1, 0 TSRMLS_CC); }
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
