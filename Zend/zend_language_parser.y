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
	|	T_IF parenthesis_expr { AC($2); zend_do_if_cond(&$2, &$1 TSRMLS_CC); } statement { zend_do_if_after_statement(&$1, 1 TSRMLS_CC); } elseif_list else_single { zend_do_if_end(TSRMLS_C); }
	|	T_IF parenthesis_expr ':' { AC($2); zend_do_if_cond(&$2, &$1 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$1, 1 TSRMLS_CC); } new_elseif_list new_else_single T_ENDIF ';' { zend_do_if_end(TSRMLS_C); }
	|	T_WHILE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); } parenthesis_expr { AC($3); zend_do_while_cond(&$3, &$$ TSRMLS_CC); } while_statement { zend_do_while_end(&$1, &$4 TSRMLS_CC); }
	|	T_DO { $1.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); } statement T_WHILE { $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); } parenthesis_expr ';' { AC($6); zend_do_do_while_end(&$1, &$4, &$6 TSRMLS_CC); }
	|	T_FOR
			'('
				for_expr
			';' { zend_do_free(&$3 TSRMLS_CC); $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); }
				for_expr
			';' { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&$6, &$7 TSRMLS_CC); }
				for_expr
			')' { zend_do_free(&$9 TSRMLS_CC); zend_do_for_before_statement(&$4, &$7 TSRMLS_CC); }
			for_statement { zend_do_for_end(&$7 TSRMLS_CC); }
	|	T_SWITCH parenthesis_expr	{ AC($2); zend_do_switch_cond(&$2 TSRMLS_CC); } switch_case_list { zend_do_switch_end(&$4 TSRMLS_CC); }
	|	T_BREAK ';'				{ zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
	|	T_BREAK expr ';'		{ AC($2); zend_do_brk_cont(ZEND_BRK, &$2 TSRMLS_CC); }
	|	T_CONTINUE ';'			{ zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
	|	T_CONTINUE expr ';'		{ AC($2); zend_do_brk_cont(ZEND_CONT, &$2 TSRMLS_CC); }
	|	T_RETURN ';'						{ zend_do_return(NULL, 0 TSRMLS_CC); }
	|	T_RETURN expr_without_variable ';'	{ AC($2); zend_do_return(&$2, 0 TSRMLS_CC); }
	|	T_RETURN variable ';'				{ zend_do_return(&$2, 1 TSRMLS_CC); }
	|	T_GLOBAL global_var_list ';'
	|	T_STATIC static_var_list ';'
	|	T_ECHO echo_expr_list ';'
	|	T_INLINE_HTML			{ zend_do_echo(&$1 TSRMLS_CC); }
	|	expr ';'				{ AC($1); zend_do_free(&$1 TSRMLS_CC); }
	|	T_UNSET '(' unset_variables ')' ';'
	|	T_FOREACH '(' variable T_AS
		{ zend_do_foreach_begin(&$1, &$2, &$3, &$4, 1 TSRMLS_CC); }
		foreach_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
	|	T_FOREACH '(' expr_without_variable T_AS
		{ AC($3); zend_do_foreach_begin(&$1, &$2, &$3, &$4, 0 TSRMLS_CC); }
		foreach_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
	|	T_DECLARE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); } '(' declare_list ')' declare_statement { zend_do_declare_end(&$1 TSRMLS_CC); }
	|	';'		/* empty statement */
	|	T_TRY { zend_do_try(&$1 TSRMLS_CC); } '{' inner_statement_list '}'
		catch_statement { zend_do_bind_catch(&$1, &$6 TSRMLS_CC); }
		finally_statement { zend_do_end_finally(&$1, &$6, &$8 TSRMLS_CC); }
	|	T_THROW expr ';' { AC($2); zend_do_throw(&$2 TSRMLS_CC); }
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
		variable
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_UNSET, $1.u.ast); AST_COMPILE_STMT($$.u.ast); }
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
		variable			{ $$.u.ast = $1.u.ast; $$.EA = 0; }
	|	'&' variable		{ $$.u.ast = $2.u.ast; $$.EA = ZEND_PARSED_REFERENCE_VARIABLE; }
	|	T_LIST '(' assignment_list ')' { $$.u.ast = $3.u.ast; $$.EA = ZEND_PARSED_LIST_EXPR; }
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
	|	case_list T_CASE expr case_separator { AC($3); zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&$1, &$2, &$3 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
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
	|	elseif_list T_ELSEIF parenthesis_expr { AC($3); zend_do_if_cond(&$3, &$2 TSRMLS_CC); } statement { zend_do_if_after_statement(&$2, 0 TSRMLS_CC); }
;


new_elseif_list:
		/* empty */
	|	new_elseif_list T_ELSEIF parenthesis_expr ':' { AC($3); zend_do_if_cond(&$3, &$2 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$2, 0 TSRMLS_CC); }
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
	|	T_ARRAY						{ $$.op_type = IS_CONST; Z_TYPE_INFO($$.u.constant)=IS_ARRAY; }
	|	T_CALLABLE					{ $$.op_type = IS_CONST; Z_TYPE_INFO($$.u.constant)=IS_CALLABLE; }
	|	fully_qualified_class_name			{ $$ = $1; }
;

function_call_parameter_list:
		'(' ')'	{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_PARAMS); }
	|	'(' non_empty_function_call_parameter_list ')' { $$.u.ast = $2.u.ast; }
;

non_empty_function_call_parameter_list:
		function_call_parameter
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_PARAMS, $1.u.ast); }
	|	non_empty_function_call_parameter_list ',' function_call_parameter
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
;

function_call_parameter:
		expr_without_variable	{ $$.u.ast = $1.u.ast; }
	|	variable				{ $$.u.ast = $1.u.ast; }
	/*|	'&' variable 			{ ZEND_ASSERT(0); } */
	|	T_ELLIPSIS expr			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_UNPACK, $2.u.ast); }
;

global_var_list:
		global_var_list ',' global_var
	|	global_var
;

global_var:
	simple_variable
		{ $$.u.ast = zend_ast_create_unary(ZEND_AST_GLOBAL, $1.u.ast); AST_COMPILE_STMT($$.u.ast); }
;


static_var_list:
		static_var_list ',' T_VARIABLE { zend_do_fetch_static_variable(&$3, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	static_var_list ',' T_VARIABLE '=' static_scalar { Z_CONST_FLAGS($5.u.constant) = 0; zend_do_fetch_static_variable(&$3, &$5, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE  { zend_do_fetch_static_variable(&$1, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE '=' static_scalar { Z_CONST_FLAGS($3.u.constant) = 0; zend_do_fetch_static_variable(&$1, &$3, ZEND_FETCH_STATIC TSRMLS_CC); }

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
		fully_qualified_class_name									{ zend_resolve_class_name(&$1 TSRMLS_CC); zend_init_list(&$$.u.op.ptr, Z_STR($1.u.constant) TSRMLS_CC); }
	|	trait_reference_list ',' fully_qualified_class_name			{ zend_resolve_class_name(&$3 TSRMLS_CC); zend_add_to_list(&$1.u.op.ptr, Z_STR($3.u.constant) TSRMLS_CC); $$ = $1; }
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
		echo_expr_list ',' expr { AC($3); zend_do_echo(&$3 TSRMLS_CC); }
	|	expr					{ AC($1); zend_do_echo(&$1 TSRMLS_CC); }
;


for_expr:
		/* empty */			{ $$.op_type = IS_CONST;  ZVAL_BOOL(&$$.u.constant, 1); }
	|	non_empty_for_expr	{ $$ = $1; }
;

non_empty_for_expr:
		non_empty_for_expr ','	{ zend_do_free(&$1 TSRMLS_CC); } expr { AC($4); $$ = $4; }
	|	expr					{ AC($1); $$ = $1; }
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
			{ $$.u.ast = zend_ast_create_binary(ZEND_BOOL_XOR, $1.u.ast, $3.u.ast); }
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
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_LONG, $2.u.ast); }
	|	T_DOUBLE_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_DOUBLE, $2.u.ast); }
	|	T_STRING_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_STRING, $2.u.ast); }
	|	T_ARRAY_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_ARRAY, $2.u.ast); }
	|	T_OBJECT_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_OBJECT, $2.u.ast); }
	|	T_BOOL_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, _IS_BOOL, $2.u.ast); }
	|	T_UNSET_CAST expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_AST_CAST, IS_NULL, $2.u.ast); }
	|	T_EXIT exit_expr { $$.u.ast = zend_ast_create_unary(ZEND_EXIT, $2.u.ast); }
	|	'@' expr { $$.u.ast = zend_ast_create_unary(ZEND_AST_SILENCE, $2.u.ast); }
	|	scalar { $$.u.ast = $1.u.ast; }
	|	'`' backticks_expr '`' { $$.u.ast = zend_ast_create_unary(ZEND_AST_SHELL_EXEC, $2.u.ast); }
	|	T_PRINT expr { $$.u.ast = zend_ast_create_unary(ZEND_PRINT, $2.u.ast); }
	|	T_YIELD { $$.u.ast = zend_ast_create_binary(ZEND_YIELD, NULL, NULL); }
	|	T_YIELD expr { $$.u.ast = zend_ast_create_binary(ZEND_YIELD, $2.u.ast, NULL); }
	|	T_YIELD expr T_DOUBLE_ARROW expr
			{ $$.u.ast = zend_ast_create_binary(ZEND_YIELD, $4.u.ast, $2.u.ast); }
	|	function is_reference { zend_do_begin_lambda_function_declaration(&$$, &$1, $2.op_type, 0 TSRMLS_CC); }
		'(' parameter_list ')' lexical_vars
		'{' inner_statement_list '}' { zend_do_end_function_declaration(&$1 TSRMLS_CC); $$.u.ast = AST_ZNODE(&$3); }
	|	T_STATIC function is_reference { zend_do_begin_lambda_function_declaration(&$$, &$2, $3.op_type, 1 TSRMLS_CC); }
		'(' parameter_list ')' lexical_vars
		'{' inner_statement_list '}' { zend_do_end_function_declaration(&$2 TSRMLS_CC); $$.u.ast = AST_ZNODE(&$4); }
;

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
		namespace_name function_call_parameter_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL,
			  	  zend_ast_create_zval_ex(&$1.u.constant, 1), $2.u.ast); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name function_call_parameter_list
			{ ZVAL_EMPTY_STRING(&$1.u.constant);
			  zend_do_build_namespace_name(&$1, &$1, &$3 TSRMLS_CC);
			  $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL, AST_ZVAL(&$1), $4.u.ast); }
	|	T_NS_SEPARATOR namespace_name function_call_parameter_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL, AST_ZVAL(&$2), $3.u.ast); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM member_name function_call_parameter_list
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      AST_ZVAL(&$1), $3.u.ast, $4.u.ast); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM member_name function_call_parameter_list
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_STATIC_CALL,
			      $1.u.ast, $3.u.ast, $4.u.ast); }
	|	callable_expr function_call_parameter_list
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_CALL, $1.u.ast, $2.u.ast); }
;

class_name:
		T_STATIC { $$.op_type = IS_CONST; ZVAL_STRINGL(&$$.u.constant, "static", sizeof("static")-1);}
	|	namespace_name { $$ = $1; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { zval tmp; ZVAL_NEW_STR(&tmp, STR_ALLOC(Z_STRLEN($2.u.constant)+1, 0)); Z_STRVAL(tmp)[0] = '\\'; memcpy(Z_STRVAL(tmp) + 1, Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); if (Z_DELREF($2.u.constant) == 0) {efree(Z_STR($2.u.constant));} Z_STR($2.u.constant) = Z_STR(tmp); $$ = $2; }
;

fully_qualified_class_name:
		namespace_name { $$ = $1; }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
	|	T_NS_SEPARATOR namespace_name { zval tmp; ZVAL_NEW_STR(&tmp, STR_ALLOC(Z_STRLEN($2.u.constant)+1, 0)); Z_STRVAL(tmp)[0] = '\\'; memcpy(Z_STRVAL(tmp) + 1, Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); if (Z_DELREF($2.u.constant) == 0) {efree(Z_STR($2.u.constant));} Z_STR($2.u.constant) = Z_STR(tmp); $$ = $2; }
;



class_name_reference:
		class_name		{ $$.u.ast = AST_ZVAL(&$1); }
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
			  $$.u.ast = zend_ast_create_constant(&empty_str); }
	|	T_ENCAPSED_AND_WHITESPACE { $$.u.ast = AST_ZVAL(&$1); }
	|	encaps_list { $$.u.ast = $1.u.ast; }
;


ctor_arguments:
		/* empty */	{ $$.u.ast = zend_ast_create_dynamic(ZEND_AST_PARAMS); }
	|	function_call_parameter_list { $$.u.ast = $1.u.ast; }
;


dereferencable_scalar:
		T_ARRAY '(' array_pair_list ')'	{ $$.u.ast = $3.u.ast; }
	|	'[' array_pair_list ']'			{ $$.u.ast = $2.u.ast; }
	|	T_CONSTANT_ENCAPSED_STRING		{ $$.u.ast = AST_ZVAL(&$1); }
;

common_scalar:
		T_LNUMBER 					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_DNUMBER 					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_LINE 						{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_FILE 						{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_DIR   					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_TRAIT_C					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_METHOD_C					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_FUNC_C					{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_NS_C						{ $$.u.ast = AST_ZVAL(&$1); }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$.u.ast = AST_ZVAL(&$2); }
	|	T_START_HEREDOC T_END_HEREDOC
			{ zval empty_str; ZVAL_EMPTY_STRING(&empty_str);
			  $$.u.ast = zend_ast_create_constant(&empty_str); }
;

static_class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_CT, 0 TSRMLS_CC); }
;

static_scalar: /* compile-time evaluated scalars */
	static_scalar_value { zend_do_constant_expression(&$$, $1.u.ast TSRMLS_CC); }
;

static_scalar_value:
		T_CONSTANT_ENCAPSED_STRING	{ $$.u.ast = AST_ZVAL(&$1); }
	|	static_class_name_scalar	{ $$.u.ast = AST_ZVAL(&$1); }
	|	static_class_constant		{ $$.u.ast = AST_ZVAL(&$1); }
	|	namespace_name 		{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_CT, 1 TSRMLS_CC); $$.u.ast = AST_ZVAL(&$$); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_CT, 0 TSRMLS_CC); $$.u.ast = AST_ZVAL(&$$); }
	|	T_NS_SEPARATOR namespace_name { zval tmp; ZVAL_NEW_STR(&tmp, STR_ALLOC(Z_STRLEN($2.u.constant)+1, 0)); Z_STRVAL(tmp)[0] = '\\'; memcpy(Z_STRVAL(tmp) + 1, Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); if (Z_DELREF($2.u.constant) == 0) {efree(Z_STR($2.u.constant));} Z_STR($2.u.constant) = Z_STR(tmp); zend_do_fetch_constant(&$$, NULL, &$2, ZEND_CT, 0 TSRMLS_CC); $$.u.ast = AST_ZVAL(&$$); }
	|	common_scalar { $$.u.ast = $1.u.ast; }
	|	T_ARRAY '(' static_array_pair_list ')' { $$.u.ast = $3.u.ast; }
	|	'[' static_array_pair_list ']' { $$.u.ast = $2.u.ast; }
	|	static_operation { $$.u.ast = $1.u.ast; }
	|	T_CLASS_C
			{ if (Z_TYPE($1.u.constant) == IS_UNDEF) {
			      zval class_const;
			      ZVAL_STRING(&class_const, "__CLASS__");
			      Z_TYPE_INFO(class_const) = IS_CONSTANT_EX;
			      $$.u.ast = zend_ast_create_constant(&class_const);
			  } else {
			      $$.u.ast = AST_ZVAL(&$1);
			  } }
;

static_operation:
		'!' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_BOOL_NOT, $2.u.ast); }
	|	'~' static_scalar_value { $$.u.ast = zend_ast_create_unary(ZEND_BW_NOT, $2.u.ast); }
	|	static_scalar_value '+' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_ADD, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '-' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_SUB, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '*' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_MUL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_POW static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_POW, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '/' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_DIV, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '%' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_MOD, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '|' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '&' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '^' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_BW_XOR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_SL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_SL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_SR static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_SR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '.' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_CONCAT, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_LOGICAL_XOR static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_BOOL_XOR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_IDENTICAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_NOT_IDENTICAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_NOT_IDENTICAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_EQUAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_NOT_EQUAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_NOT_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '<' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '>' static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER, $3.u.ast, $1.u.ast); }
	|	static_scalar_value T_IS_SMALLER_OR_EQUAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_IS_GREATER_OR_EQUAL static_scalar_value
			{ $$.u.ast = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $3.u.ast, $1.u.ast); }
	|	static_scalar_value T_LOGICAL_AND static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_LOGICAL_OR static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_BOOLEAN_AND static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_AND, $1.u.ast, $3.u.ast); }
	|	static_scalar_value T_BOOLEAN_OR static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_OR, $1.u.ast, $3.u.ast); }
	|	static_scalar_value '?' ':' static_scalar_value
			{ $$.u.ast = zend_ast_create_ternary(ZEND_AST_CONDITIONAL, $1.u.ast, NULL, $4.u.ast); }
	|	static_scalar_value '?' static_scalar_value ':' static_scalar_value
			{ $$.u.ast = zend_ast_create_ternary(
			      ZEND_AST_CONDITIONAL, $1.u.ast, $3.u.ast, $5.u.ast); }
	|	'+' static_scalar_value
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_UNARY_PLUS, $2.u.ast); }
	|	'-' static_scalar_value
			{ $$.u.ast = zend_ast_create_unary(ZEND_AST_UNARY_MINUS, $2.u.ast); }
	|	'(' static_scalar_value ')' { $$.u.ast = $2.u.ast; }
;


scalar:
		class_name_scalar	{ $$.u.ast = $1.u.ast; }
	|	class_constant		{ $$.u.ast = $1.u.ast; }
	|	namespace_name
			{ $$.u.ast = zend_ast_create_unary(
			      ZEND_AST_CONST, zend_ast_create_zval_ex(&$1.u.constant, 1)); }
	|	T_NAMESPACE T_NS_SEPARATOR namespace_name
			{ ZVAL_EMPTY_STRING(&$1.u.constant);
			  zend_do_build_namespace_name(&$1, &$1, &$3 TSRMLS_CC);
			  $$.u.ast = zend_ast_create_unary(ZEND_AST_CONST, AST_ZVAL(&$1)); }
	|	T_NS_SEPARATOR namespace_name
			{ zval tmp; ZVAL_NEW_STR(&tmp, STR_ALLOC(Z_STRLEN($2.u.constant)+1, 0)); Z_STRVAL(tmp)[0] = '\\'; memcpy(Z_STRVAL(tmp) + 1, Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1);
			  if (Z_DELREF($2.u.constant) == 0) { efree(Z_STR($2.u.constant)); }
			  Z_STR($2.u.constant) = Z_STR(tmp);
			  $$.u.ast = zend_ast_create_unary(ZEND_AST_CONST, AST_ZVAL(&$2)); }
	|	common_scalar			{ $$.u.ast = $1.u.ast; }
	|	'"' encaps_list '"' 	{ $$.u.ast = $2.u.ast; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$.u.ast = $2.u.ast; }
	|	T_CLASS_C
			{ if (Z_TYPE($1.u.constant) == IS_UNDEF) {
			      zval class_const; ZVAL_STRING(&class_const, "__CLASS__");
			      $$.u.ast = zend_ast_create_unary(ZEND_AST_CONST,
				      zend_ast_create_constant(&class_const));
			  } else {
			      $$.u.ast = AST_ZVAL(&$1);
			  } }
	|	dereferencable_scalar	{ $$.u.ast = $1.u.ast; }
;


static_array_pair_list:
		/* empty */
			{ array_init(&$$.u.constant); $$.u.ast = zend_ast_create_constant(&$$.u.constant); }
	|	non_empty_static_array_pair_list possible_comma
			{ zend_ast_dynamic_shrink(&$1.u.ast); $$ = $1; }
;

possible_comma:
		/* empty */
	|	','
;

non_empty_static_array_pair_list:
		non_empty_static_array_pair_list ',' static_array_pair
			{ $$.u.ast = zend_ast_dynamic_add($1.u.ast, $3.u.ast); }
	|	static_array_pair
			{ $$.u.ast = zend_ast_create_dynamic_and_add(ZEND_AST_ARRAY, $1.u.ast); }
;

static_array_pair:
		static_scalar_value T_DOUBLE_ARROW static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $3.u.ast, $1.u.ast); }
	|	static_scalar_value
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_ARRAY_ELEM, $1.u.ast, NULL); }
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
	|	dereferencable T_OBJECT_OPERATOR member_name function_call_parameter_list
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
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, AST_ZVAL(&$1), $3.u.ast); }
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
			{ $$.u.ast = zend_ast_create_binary(ZEND_AST_STATIC_PROP, AST_ZVAL(&$1), $3.u.ast); }
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
			{ $$.u.ast = zend_ast_create_binary_ex(ZEND_AST_ARRAY_ELEM, 1, $4.u.ast, $1.u.ast); }
	|	'&' variable
			{ $$.u.ast = zend_ast_create_binary_ex(ZEND_AST_ARRAY_ELEM, 1, $2.u.ast, NULL); }
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
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_INCLUDE_OR_EVAL, ZEND_INCLUDE, $2.u.ast); }
	|	T_INCLUDE_ONCE expr
			{ $$.u.ast = zend_ast_create_unary_ex(
			      ZEND_INCLUDE_OR_EVAL, ZEND_INCLUDE_ONCE, $2.u.ast); }
	|	T_EVAL '(' expr ')'
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_INCLUDE_OR_EVAL, ZEND_EVAL, $3.u.ast); }
	|	T_REQUIRE expr
			{ $$.u.ast = zend_ast_create_unary_ex(ZEND_INCLUDE_OR_EVAL, ZEND_REQUIRE, $2.u.ast); }
	|	T_REQUIRE_ONCE expr
			{ $$.u.ast = zend_ast_create_unary_ex(
			      ZEND_INCLUDE_OR_EVAL, ZEND_REQUIRE_ONCE, $2.u.ast); }
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
			      ZEND_AST_CLASS_CONST, AST_ZVAL(&$1), AST_ZVAL(&$3)); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM T_STRING
			{ $$.u.ast = zend_ast_create_binary(
			      ZEND_AST_CLASS_CONST, $1.u.ast, AST_ZVAL(&$3)); }
;

static_class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS { zend_do_resolve_class_name(&$$, &$1, 1 TSRMLS_CC); }
;

class_name_scalar:
	class_name T_PAAMAYIM_NEKUDOTAYIM T_CLASS
		{ $$.u.ast = zend_ast_create_unary(ZEND_AST_RESOLVE_CLASS_NAME, AST_ZVAL(&$1)); }
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
