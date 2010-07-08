%include{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
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

#include "zend_compile.h"
#include "zend.h"
#include "zend_list.h"
#include "zend_globals.h"
#include "zend_API.h"
#include "zend_constants.h"
}

//%name lemon_

%syntax_error {
	zend_error(E_PARSE, "syntax error, unexpected '%.*s'", LANG_SCNG(yy_leng), LANG_SCNG(yy_text));
}

%stack_overflow {
     fprintf(stderr,"Woops, parser stack overflow\n");
}

%stack_size 500

%start_symbol start

%token_prefix T_

%token_type {znode}

%extra_argument {void ***tsrm_ls}


/* TOKENS TRANSLATION:
   
   , => T_COMMA
   = => T_EQUAL
   ( => T_LPAREN
   ) => T_LPAREN
   ; => T_SEMICOLON
   : => T_COLON
   { => T_LBRACE
   } => T_LBRACE

*/

%left INCLUDE INCLUDE_ONCE EVAL REQUIRE REQUIRE_ONCE.
%left COMMA.
%left LOGICAL_OR.
%left LOGICAL_XOR.
%left LOGICAL_AND.
%right PRINT.
%left EQUAL PLUS_EQUAL MINUS_EQUAL MUL_EQUAL DIV_EQUAL CONCAT_EQUAL MOD_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL SL_EQUAL SR_EQUAL.
%left QUESTION_MARK COLON.
%left BOOLEAN_OR.
%left BOOLEAN_AND.
%left PIPE.
%left HAT.
%left AMPERSAND.
%nonassoc IS_EQUAL IS_NOT_EQUAL IS_IDENTICAL IS_NOT_IDENTICAL.
%nonassoc IS_SMALLER IS_SMALLER_OR_EQUAL IS_GREATER IS_GREATER_OR_EQUAL.
%left SL SR.
%left PLUS MINUS DOT.
%left TIMES SLASH PERCENT.
%right EXCLAMATION.
%nonassoc INSTANCEOF.
%right TILDA INC DEC INT_CAST NUMERIC_CAST SCALAR_CAST DOUBLE_CAST STRING_CAST ARRAY_CAST OBJECT_CAST BOOL_CAST UNSET_CAST AT.
%right RBRACKET.
%nonassoc NEW CLONE.
%nonassoc EXIT.
%nonassoc IF.
%left ELSEIF.
%left ELSE.
%left ENDIF.

%right STATIC ABSTRACT FINAL PRIVATE PROTECTED PUBLIC.

//start:
//	top_statement_list	{ zend_do_end_compilation(TSRMLS_C); }
//;
start ::= top_statement_list. { zend_do_end_compilation(TSRMLS_C); }

//top_statement_list:
//		top_statement_list  { zend_do_extended_info(TSRMLS_C); } top_statement { HANDLE_INTERACTIVE(); }
//	|	/* empty */
//;
top_statement_list ::= top_statement_list_i top_statement. { HANDLE_INTERACTIVE(); }
top_statement_list ::= .

top_statement_list_i ::= top_statement_list. { zend_do_extended_info(TSRMLS_C); }

//
//namespace_name:
//		STRING { $$ = $1; }
//	|	namespace_name NS_SEPARATOR STRING { zend_do_build_namespace_name(&$$, &$1, &$3 TSRMLS_CC); }
//;

namespace_name(A) ::= STRING(B). { A = B; }
namespace_name(A) ::= namespace_name(B) NS_SEPARATOR STRING(C). { zend_do_build_namespace_name(&A, &B, &C TSRMLS_CC); }

//
//top_statement:
//		statement						{ zend_verify_namespace(TSRMLS_C); }
//	|	function_declaration_statement	{ zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
//	|	class_declaration_statement		{ zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
//	|	HALT_COMPILER LPAREN RPAREN SEMICOLON		{ zend_do_halt_compiler_register(TSRMLS_C); YYACCEPT; }
//	|	NAMESPACE namespace_name SEMICOLON	{ zend_do_begin_namespace(&$2, 0 TSRMLS_CC); }
//	|	NAMESPACE namespace_name LBRACE	{ zend_do_begin_namespace(&$2, 1 TSRMLS_CC); }
//		top_statement_list RBRACE		    { zend_do_end_namespace(TSRMLS_C); }
//	|	NAMESPACE LBRACE					{ zend_do_begin_namespace(NULL, 1 TSRMLS_CC); }
//		top_statement_list RBRACE			{ zend_do_end_namespace(TSRMLS_C); }
//	|	USE use_declarations SEMICOLON      { zend_verify_namespace(TSRMLS_C); }
//	|	constant_declaration SEMICOLON		{ zend_verify_namespace(TSRMLS_C); }
//;

top_statement ::= statement.                                       { zend_verify_namespace(TSRMLS_C); }
//top_statement ::= function_declaration_statement.                  { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
//top_statement ::= class_declaration_statement.                     { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
top_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON.   { zend_do_halt_compiler_register(TSRMLS_C); /* YYACCEPT; */ }
top_statement ::= NAMESPACE namespace_name(B) SEMICOLON.       { zend_do_begin_namespace(&B, 0 TSRMLS_CC); }
top_statement ::= NAMESPACE namespace_name(B) LBRACE.          { zend_do_begin_namespace(&B, 1 TSRMLS_CC); }
top_statement ::= top_statement_list RBRACE.                     { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= NAMESPACE LBRACE.                            { zend_do_begin_namespace(NULL, 1 TSRMLS_CC); }
top_statement ::= top_statement_list RBRACE.                     { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= USE use_declarations SEMICOLON.              { zend_verify_namespace(TSRMLS_C); }
top_statement ::= constant_declaration SEMICOLON.                { zend_verify_namespace(TSRMLS_C); }
// Just to make the build works
top_statement ::= todo.

//
//use_declarations:
//		use_declarations COMMA use_declaration
//	|	use_declaration
//;

use_declarations ::= use_declarations COMMA use_declaration.
use_declarations ::= use_declaration.

//
//use_declaration:
//		namespace_name 			{ zend_do_use(&$1, NULL, 0 TSRMLS_CC); }
//	|	namespace_name AS STRING	{ zend_do_use(&$1, &$3, 0 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { zend_do_use(&$2, NULL, 1 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name AS STRING { zend_do_use(&$2, &$4, 1 TSRMLS_CC); }
//;

use_declaration ::= namespace_name(B).                                 { zend_do_use(&B, NULL, 0 TSRMLS_CC); }
use_declaration ::= namespace_name(B) AS STRING(C).                { zend_do_use(&B, &C, 0 TSRMLS_CC); }
use_declaration ::= NS_SEPARATOR namespace_name(B).                  { zend_do_use(&B, NULL, 1 TSRMLS_CC); }
use_declaration ::= NS_SEPARATOR namespace_name(B) AS STRING(C). { zend_do_use(&B, &C, 1 TSRMLS_CC); }

//
//constant_declaration:
//		constant_declaration COMMA STRING EQUAL static_scalar	{ zend_do_declare_constant(&$3, &$5 TSRMLS_CC); }
//	|	CONST STRING EQUAL static_scalar { zend_do_declare_constant(&$2, &$4 TSRMLS_CC); }
//;

constant_declaration ::= constant_declaration COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_constant(&B, &C TSRMLS_CC); }
constant_declaration ::= CONST STRING(B) EQUAL static_scalar(C).                      { zend_do_declare_constant(&B, &C TSRMLS_CC); }

//
//inner_statement_list:
//		inner_statement_list  { zend_do_extended_info(TSRMLS_C); } inner_statement { HANDLE_INTERACTIVE(); }
//	|	/* empty */
//;

inner_statement_list ::= inner_statement_list_i inner_statement. { HANDLE_INTERACTIVE(); }
inner_statement_list ::= .

inner_statement_list_i ::= inner_statement_list. { zend_do_extended_info(TSRMLS_C); }

//
//
//inner_statement:
//		statement
//	|	function_declaration_statement
//	|	class_declaration_statement
//	|	HALT_COMPILER LPAREN RPAREN SEMICOLON   { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
//;


inner_statement ::= statement.
//inner_statement ::= function_declaration_statement.
//inner_statement ::= class_declaration_statement.
inner_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON. { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }

//
//
//statement:
//		unticked_statement { zend_do_ticks(TSRMLS_C); }
//	|	STRING COLON { zend_do_label(&$1 TSRMLS_CC); }
//;

statement ::= unticked_statement.  { zend_do_ticks(TSRMLS_C); }
statement ::= STRING(A) COLON. { zend_do_label(&A TSRMLS_CC); }

//
//unticked_statement:
//		LBRACE inner_statement_list RBRACE
//	|	IF LPAREN expr RPAREN { zend_do_if_cond(&$3, &$4 TSRMLS_CC); } statement { zend_do_if_after_statement(&$4, 1 TSRMLS_CC); } elseif_list else_single { zend_do_if_end(TSRMLS_C); }
//	|	IF LPAREN expr RPAREN COLON { zend_do_if_cond(&$3, &$4 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$4, 1 TSRMLS_CC); } new_elseif_list else_alt_single ENDIF SEMICOLON { zend_do_if_end(TSRMLS_C); }
//	|	WHILE LPAREN { $1.u.op.opline_num = get_next_op_number(CG(active_op_array));  } expr  RPAREN { zend_do_while_cond(&$4, &$5 TSRMLS_CC); } while_statement { zend_do_while_end(&$1, &$5 TSRMLS_CC); }
//	|	DO { $1.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); } statement WHILE LPAREN { $5.u.op.opline_num = get_next_op_number(CG(active_op_array)); } expr RPAREN SEMICOLON { zend_do_do_while_end(&$1, &$5, &$7 TSRMLS_CC); }
//	|	FOR
//			LPAREN
//				for_expr
//			SEMICOLON { zend_do_free(&$3 TSRMLS_CC); $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); }
//				for_expr
//			SEMICOLON { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&$6, &$7 TSRMLS_CC); }
//				for_expr
//			RPAREN { zend_do_free(&$9 TSRMLS_CC); zend_do_for_before_statement(&$4, &$7 TSRMLS_CC); }
//			for_statement { zend_do_for_end(&$7 TSRMLS_CC); }
//	|	SWITCH LPAREN expr RPAREN	{ zend_do_switch_cond(&$3 TSRMLS_CC); } switch_case_list { zend_do_switch_end(&$6 TSRMLS_CC); }
//	|	BREAK SEMICOLON				{ zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
//	|	BREAK expr SEMICOLON		{ zend_do_brk_cont(ZEND_BRK, &$2 TSRMLS_CC); }
//	|	CONTINUE SEMICOLON			{ zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
//	|	CONTINUE expr SEMICOLON		{ zend_do_brk_cont(ZEND_CONT, &$2 TSRMLS_CC); }
//	|	RETURN SEMICOLON						{ zend_do_return(NULL, 0 TSRMLS_CC); }
//	|	RETURN expr_without_variable SEMICOLON	{ zend_do_return(&$2, 0 TSRMLS_CC); }
//	|	RETURN variable SEMICOLON				{ zend_do_return(&$2, 1 TSRMLS_CC); }
//	|	GLOBAL global_var_list SEMICOLON
//	|	STATIC static_var_list SEMICOLON
//	|	ECHO echo_expr_list SEMICOLON
//	|	INLINE_HTML			{ zend_do_echo(&$1 TSRMLS_CC); }
//	|	expr SEMICOLON				{ zend_do_free(&$1 TSRMLS_CC); }
//	|	UNSET LPAREN unset_variables RPAREN SEMICOLON
//	|	FOREACH LPAREN variable AS
//		{ zend_do_foreach_begin(&$1, &$2, &$3, &$4, 1 TSRMLS_CC); }
//		foreach_variable foreach_optional_arg RPAREN { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
//		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
//	|	FOREACH LPAREN expr_without_variable AS
//		{ zend_do_foreach_begin(&$1, &$2, &$3, &$4, 0 TSRMLS_CC); }
//		variable foreach_optional_arg RPAREN { zend_check_writable_variable(&$6); zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
//		foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
//	|	DECLARE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); } LPAREN declare_list RPAREN declare_statement { zend_do_declare_end(&$1 TSRMLS_CC); }
//	|	SEMICOLON		/* empty statement */
//	|	TRY { zend_do_try(&$1 TSRMLS_CC); } LBRACE inner_statement_list RBRACE
//		CATCH LPAREN { zend_initialize_try_catch_element(&$1 TSRMLS_CC); }
//		fully_qualified_class_name { zend_do_first_catch(&$7 TSRMLS_CC); }
//		VARIABLE RPAREN { zend_do_begin_catch(&$1, &$9, &$11, &$7 TSRMLS_CC); }
//		LBRACE inner_statement_list RBRACE { zend_do_end_catch(&$1 TSRMLS_CC); }
//		additional_catches { zend_do_mark_last_catch(&$7, &$18 TSRMLS_CC); }
//	|	THROW expr SEMICOLON { zend_do_throw(&$2 TSRMLS_CC); }
//	|	GOTO STRING SEMICOLON { zend_do_goto(&$2 TSRMLS_CC); }
//;

if_cond(A)   ::= IF LPAREN expr(B) RPAREN(C). { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }
if_cond_then ::= if_cond(B) statement.              { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

if_alt_cond(A)   ::= IF LPAREN expr(B) RPAREN(C) COLON. { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }
if_alt_cond_then ::= if_alt_cond(B) inner_statement_list.       { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

unticked_statement ::= LBRACE inner_statement_list RBRACE.
//unticked_statement ::= if_cond_then elseif_list else_single. { zend_do_if_end(TSRMLS_C); }

//unticked_statement ::= if_alt_cond_then elseif_alt_list else_alt_single ENDIF SEMICOLON. { zend_do_if_end(TSRMLS_C); }

while_begin ::= WHILE(B) LPAREN. { B.u.op.opline_num = get_next_op_number(CG(active_op_array)); }

/* FIXME
unticked_statement ::= while_begin  expr RPAREN { zend_do_while_cond(&$4, &$5 TSRMLS_CC); } while_statement { zend_do_while_end(&$1, &$5 TSRMLS_CC); }
unticked_statement ::= DO { $1.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); } statement WHILE LPAREN { $5.u.op.opline_num = get_next_op_number(CG(active_op_array)); } expr RPAREN SEMICOLON { zend_do_do_while_end(&$1, &$5, &$7 TSRMLS_CC); }
unticked_statement ::= FOR
                        LPAREN
                            for_expr
                        SEMICOLON { zend_do_free(&$3 TSRMLS_CC); $4.u.op.opline_num = get_next_op_number(CG(active_op_array)); }
                            for_expr
                        SEMICOLON { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&$6, &$7 TSRMLS_CC); }
                            for_expr
                        RPAREN { zend_do_free(&$9 TSRMLS_CC); zend_do_for_before_statement(&$4, &$7 TSRMLS_CC); }
                        for_statement { zend_do_for_end(&$7 TSRMLS_CC); }
unticked_statement ::= SWITCH LPAREN expr RPAREN	{ zend_do_switch_cond(&$3 TSRMLS_CC); } switch_case_list { zend_do_switch_end(&$6 TSRMLS_CC); }
*/
unticked_statement ::= BREAK SEMICOLON.            { zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
unticked_statement ::= BREAK expr(B) SEMICOLON.    { zend_do_brk_cont(ZEND_BRK, &B TSRMLS_CC); }
unticked_statement ::= CONTINUE SEMICOLON.         { zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
unticked_statement ::= CONTINUE expr(B) SEMICOLON. { zend_do_brk_cont(ZEND_CONT, &B TSRMLS_CC); }

unticked_statement ::= RETURN SEMICOLON.                            { zend_do_return(NULL, 0 TSRMLS_CC); }
unticked_statement ::= RETURN expr_without_variable(B) SEMICOLON.   { zend_do_return(&B, 0 TSRMLS_CC); }
//unticked_statement ::= RETURN variable(B) SEMICOLON.                { zend_do_return(&(B), 1 TSRMLS_CC); }

//unticked_statement ::= GLOBAL global_var_list SEMICOLON.
//unticked_statement ::= STATIC static_var_list SEMICOLON.
unticked_statement ::= ECHO echo_expr_list SEMICOLON.
unticked_statement ::= INLINE_HTML(B). { zend_do_echo(&B TSRMLS_CC); }

unticked_statement ::= expr(B) SEMICOLON. { zend_do_free(&B TSRMLS_CC); }
//unticked_statement ::= UNSET LPAREN unset_variables RPAREN SEMICOLON.
/* FIXME
unticked_statement ::= FOREACH LPAREN variable AS
                       { zend_do_foreach_begin(&$1, &$2, &$3, &$4, 1 TSRMLS_CC); }
                       foreach_variable foreach_optional_arg RPAREN { zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
                       foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
unticked_statement ::= FOREACH LPAREN expr_without_variable AS
                   ::= { zend_do_foreach_begin(&$1, &$2, &$3, &$4, 0 TSRMLS_CC); }
                   ::= variable foreach_optional_arg RPAREN { zend_check_writable_variable(&$6); zend_do_foreach_cont(&$1, &$2, &$4, &$6, &$7 TSRMLS_CC); }
                   ::= foreach_statement { zend_do_foreach_end(&$1, &$4 TSRMLS_CC); }
unticked_statement ::= DECLARE { $1.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); } LPAREN declare_list RPAREN declare_statement { zend_do_declare_end(&$1 TSRMLS_CC); }
*/

unticked_statement ::= SEMICOLON. /* empty statement */
/* FIXME
try ::= TRY(B). { zend_do_try(&B TSRMLS_CC); }
unticked_statement ::= try LBRACE inner_statement_list RBRACE.
                       CATCH(B) LPAREN. { zend_initialize_try_catch_element(&B TSRMLS_CC); }
                       fully_qualified_class_name. { zend_do_first_catch(&$7 TSRMLS_CC); }
                       VARIABLE RPAREN { zend_do_begin_catch(&$1, &$9, &$11, &$7 TSRMLS_CC); }
                       LBRACE inner_statement_list RBRACE { zend_do_end_catch(&$1 TSRMLS_CC); }
                       additional_catches { zend_do_mark_last_catch(&$7, &$18 TSRMLS_CC); }
*/
unticked_statement ::= THROW expr(B) SEMICOLON. { zend_do_throw(&B TSRMLS_CC); }
unticked_statement ::= GOTO STRING(B) SEMICOLON. { zend_do_goto(&B TSRMLS_CC); }

//
//
//additional_catches:
//		non_empty_additional_catches { $$ = $1; }
//	|	/* empty */ { $$.u.op.opline_num = -1; }
//;
//
//non_empty_additional_catches:
//		additional_catch { $$ = $1; }
//	|	non_empty_additional_catches additional_catch { $$ = $2; }
//;
//
//
//additional_catch:
//	CATCH LPAREN fully_qualified_class_name { $$.u.op.opline_num = get_next_op_number(CG(active_op_array)); } VARIABLE RPAREN { zend_do_begin_catch(&$1, &$3, &$5, NULL TSRMLS_CC); } LBRACE inner_statement_list RBRACE { zend_do_end_catch(&$1 TSRMLS_CC); }
//;
//
//
//unset_variables:
//		unset_variable
//	|	unset_variables COMMA unset_variable
//;
//
//unset_variable:
//		variable	{ zend_do_end_variable_parse(&$1, BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&$1 TSRMLS_CC); }
//;
//
//function_declaration_statement:
//		unticked_function_declaration_statement	{ zend_do_ticks(TSRMLS_C); }
//;
//
//class_declaration_statement:
//		unticked_class_declaration_statement	{ zend_do_ticks(TSRMLS_C); }
//;
//
//
//is_reference:
//		/* empty */	{ $$.op_type = ZEND_RETURN_VAL; }
//	|	'&'			{ $$.op_type = ZEND_RETURN_REF; }
//;
//
//
//unticked_function_declaration_statement:
//		function is_reference STRING { zend_do_begin_function_declaration(&$1, &$3, 0, $2.op_type, NULL TSRMLS_CC); }
//			LPAREN parameter_list RPAREN LBRACE inner_statement_list RBRACE { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
//;
//
//unticked_class_declaration_statement:
//		class_entry_type STRING extends_from
//			{ zend_do_begin_class_declaration(&$1, &$2, &$3 TSRMLS_CC); }
//			implements_list
//			LBRACE
//				class_statement_list
//			RBRACE { zend_do_end_class_declaration(&$1, &$2 TSRMLS_CC); }
//	|	interface_entry STRING
//			{ zend_do_begin_class_declaration(&$1, &$2, NULL TSRMLS_CC); }
//			interface_extends_list
//			LBRACE
//				class_statement_list
//			RBRACE { zend_do_end_class_declaration(&$1, &$2 TSRMLS_CC); }
//;
//
//
//class_entry_type:
//		CLASS			{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = 0; }
//	|	ABSTRACT CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
//	|	TRAIT { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_TRAIT; }
//	|	FINAL CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_FINAL_CLASS; }
//;
//
//extends_from:
//		/* empty */					{ $$.op_type = IS_UNUSED; }
//	|	EXTENDS fully_qualified_class_name	{ zend_do_fetch_class(&$$, &$2 TSRMLS_CC); }
//;
//
//interface_entry:
//	INTERFACE		{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_INTERFACE; }
//;
//
//interface_extends_list:
//		/* empty */
//	|	EXTENDS interface_list
//;
//
//implements_list:
//		/* empty */
//	|	IMPLEMENTS interface_list
//;
//
//interface_list:
//		fully_qualified_class_name			{ zend_do_implements_interface(&$1 TSRMLS_CC); }
//	|	interface_list COMMA fully_qualified_class_name { zend_do_implements_interface(&$3 TSRMLS_CC); }
//;
//
//foreach_optional_arg:
//		/* empty */						{ $$.op_type = IS_UNUSED; }
//	|	DOUBLE_ARROW foreach_variable	{ $$ = $2; }
//;
//
//
//foreach_variable:
//		variable			{ zend_check_writable_variable(&$1); $$ = $1; }
//	|	'&' variable		{ zend_check_writable_variable(&$2); $$ = $2;  $$.EA |= ZEND_PARSED_REFERENCE_VARIABLE; }
//;
//
//for_statement:
//		statement
//	|	COLON inner_statement_list ENDFOR SEMICOLON
//;
//
//
//foreach_statement:
//		statement
//	|	COLON inner_statement_list ENDFOREACH SEMICOLON
//;
//
//
//declare_statement:
//		statement
//	|	COLON inner_statement_list ENDDECLARE SEMICOLON
//;
//
//
//declare_list:
//		STRING EQUAL static_scalar					{ zend_do_declare_stmt(&$1, &$3 TSRMLS_CC); }
//	|	declare_list COMMA STRING EQUAL static_scalar	{ zend_do_declare_stmt(&$3, &$5 TSRMLS_CC); }
//;
//
//
//switch_case_list:
//		LBRACE case_list RBRACE					{ $$ = $2; }
//	|	LBRACE SEMICOLON case_list RBRACE				{ $$ = $3; }
//	|	COLON case_list ENDSWITCH SEMICOLON		{ $$ = $2; }
//	|	COLON SEMICOLON case_list ENDSWITCH SEMICOLON	{ $$ = $3; }
//;
//
//
//case_list:
//		/* empty */	{ $$.op_type = IS_UNUSED; }
//	|	case_list CASE expr case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&$1, &$2, &$3 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
//	|	case_list DEFAULT case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_default_before_statement(&$1, &$2 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
//;
//
//
//case_separator:
//		COLON
//	|	SEMICOLON
//;
//
//
//while_statement:
//		statement
//	|	COLON inner_statement_list ENDWHILE SEMICOLON
//;
//
//
//
//elseif_list:
//		/* empty */
//	|	elseif_list ELSEIF LPAREN expr RPAREN { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } statement { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
//;
//
//
//elseif_alt_list:
//		/* empty */
//	|	elseif_alt_list ELSEIF LPAREN expr RPAREN COLON { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
//;
//
//
//else_single:
//		/* empty */
//	|	ELSE statement
//;
//
//
//else_alt_single:
//		/* empty */
//	|	ELSE COLON inner_statement_list
//;
//
//
//parameter_list:
//		non_empty_parameter_list
//	|	/* empty */
//;
//
//
//non_empty_parameter_list:
//		optional_class_type VARIABLE				{ $$.op_type = IS_UNUSED; $$.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &$2, &$$, NULL, &$1, 0 TSRMLS_CC); }
//	|	optional_class_type '&' VARIABLE			{ $$.op_type = IS_UNUSED; $$.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &$3, &$$, NULL, &$1, 1 TSRMLS_CC); }
//	|	optional_class_type '&' VARIABLE EQUAL static_scalar			{ $$.op_type = IS_UNUSED; $$.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &$3, &$$, &$5, &$1, 1 TSRMLS_CC); }
//	|	optional_class_type VARIABLE EQUAL static_scalar				{ $$.op_type = IS_UNUSED; $$.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &$2, &$$, &$4, &$1, 0 TSRMLS_CC); }
//	|	non_empty_parameter_list COMMA optional_class_type VARIABLE 	{ $$=$1; $$.u.op.num++; zend_do_receive_arg(ZEND_RECV, &$4, &$$, NULL, &$3, 0 TSRMLS_CC); }
//	|	non_empty_parameter_list COMMA optional_class_type '&' VARIABLE	{ $$=$1; $$.u.op.num++; zend_do_receive_arg(ZEND_RECV, &$5, &$$, NULL, &$3, 1 TSRMLS_CC); }
//	|	non_empty_parameter_list COMMA optional_class_type '&' VARIABLE	 EQUAL static_scalar { $$=$1; $$.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &$5, &$$, &$7, &$3, 1 TSRMLS_CC); }
//	|	non_empty_parameter_list COMMA optional_class_type VARIABLE EQUAL static_scalar 	{ $$=$1; $$.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &$4, &$$, &$6, &$3, 0 TSRMLS_CC); }
//;
//
//
//optional_class_type:
//		/* empty */					{ $$.op_type = IS_UNUSED; }
//	|	ARRAY						{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_ARRAY; }
//	|	BOOL_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_BOOL; }
//	|	STRING_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_STRING; }
//	|	INT_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_LONG; }
//	|	DOUBLE_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_DOUBLE; }
//	|	RESOURCE_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_RESOURCE; }
//	|	OBJECT_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_OBJECT; }
//	|	SCALAR_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_SCALAR; }
//	|	NUMERIC_HINT					{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_NUMERIC; }
//	|	fully_qualified_class_name			{ $$ = $1; $$.op_type = IS_CONST; Z_TYPE($$.u.constant)=IS_CLASS; }
//;
//
//
//function_call_parameter_list:
//		non_empty_function_call_parameter_list	{ $$ = $1; }
//	|	/* empty */				{ Z_LVAL($$.u.constant) = 0; }
//;
//
//
//non_empty_function_call_parameter_list:
//		expr_without_variable	{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	variable				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	'&' w_variable 				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$2, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA expr_without_variable	{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA variable					{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA '&' w_variable			{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$4, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
//;
//
//global_var_list:
//		global_var_list COMMA global_var	{ zend_do_fetch_global_variable(&$3, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
//	|	global_var						{ zend_do_fetch_global_variable(&$1, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
//;
//
//
//global_var:
//		VARIABLE			{ $$ = $1; }
//	|	'$' r_variable		{ $$ = $2; }
//	|	'$' LBRACE expr RBRACE	{ $$ = $3; }
//;
//
//
//static_var_list:
//		static_var_list COMMA VARIABLE { zend_do_fetch_static_variable(&$3, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	static_var_list COMMA VARIABLE EQUAL static_scalar { zend_do_fetch_static_variable(&$3, &$5, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	VARIABLE  { zend_do_fetch_static_variable(&$1, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	VARIABLE EQUAL static_scalar { zend_do_fetch_static_variable(&$1, &$3, ZEND_FETCH_STATIC TSRMLS_CC); }
//
//;
//
//
//class_statement_list:
//		class_statement_list class_statement
//	|	/* empty */
//;
//
//
//class_statement:
//		variable_modifiers { CG(access_type) = Z_LVAL($1.u.constant); } class_variable_declaration SEMICOLON
//	|	class_constant_declaration SEMICOLON
//	|	trait_use_statement
//	|	method_modifiers function is_reference STRING { zend_do_begin_function_declaration(&$2, &$4, 1, $3.op_type, &$1 TSRMLS_CC); } LPAREN
//			parameter_list RPAREN method_body { zend_do_abstract_method(&$4, &$1, &$9 TSRMLS_CC); zend_do_end_function_declaration(&$2 TSRMLS_CC); }
//;
//
//trait_use_statement:
//		USE trait_list trait_adaptations
//;
//
//trait_list:
//		fully_qualified_class_name						{ zend_do_implements_trait(&$1 TSRMLS_CC); }
//	|	trait_list COMMA fully_qualified_class_name		{ zend_do_implements_trait(&$3 TSRMLS_CC); }
//;
//
//trait_adaptations:
//		SEMICOLON
//	|	LBRACE trait_adaptation_list RBRACE
//;
//
//trait_adaptation_list:
//		/* empty */
//	|	non_empty_trait_adaptation_list
//;
//
//non_empty_trait_adaptation_list:
//		trait_adaptation_statement
//	|	non_empty_trait_adaptation_list trait_adaptation_statement
//;
//
//trait_adaptation_statement:
//		trait_precedence SEMICOLON								{ zend_add_trait_precedence(&$1 TSRMLS_CC); }
//	|	trait_alias SEMICOLON										{ zend_add_trait_alias(&$1 TSRMLS_CC); }
//;
//
//trait_precedence:
//	trait_method_reference_fully_qualified INSTEADOF trait_reference_list	{ zend_prepare_trait_precedence(&$$, &$1, &$3 TSRMLS_CC); }
//;
//
//trait_reference_list:
//		fully_qualified_class_name									{ zend_init_list(&$$.u.op.ptr, Z_STRVAL($1.u.constant) TSRMLS_CC); }
//	|	trait_reference_list COMMA fully_qualified_class_name			{ zend_add_to_list(&$1.u.op.ptr, Z_STRVAL($3.u.constant) TSRMLS_CC); $$ = $1; }
//;
//
//trait_method_reference:
//		STRING													{ zend_prepare_reference(&$$, NULL, &$1 TSRMLS_CC); }
//	|	trait_method_reference_fully_qualified						{ $$ = $1; }
//;
//
//trait_method_reference_fully_qualified:
//	fully_qualified_class_name PAAMAYIM_NEKUDOTAYIM STRING		{ zend_prepare_reference(&$$, &$1, &$3 TSRMLS_CC); }
//;
//
//trait_alias:
//		trait_method_reference AS trait_modifiers STRING		{ zend_prepare_trait_alias(&$$, &$1, &$3, &$4 TSRMLS_CC); }
//	|	trait_method_reference AS member_modifier					{ zend_prepare_trait_alias(&$$, &$1, &$3, NULL TSRMLS_CC); }
//;
//
//trait_modifiers:
//		/* empty */					{ Z_LVAL($$.u.constant) = 0x0; } /* No change of methods visibility */
//	|	member_modifier	{ $$ = $1; } /* REM: Keep in mind, there are not only visibility modifiers */
//;
//
//method_body:
//		SEMICOLON /* abstract method */		{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
//	|	LBRACE inner_statement_list RBRACE	{ Z_LVAL($$.u.constant) = 0;	}
//;
//
//variable_modifiers:
//		non_empty_member_modifiers		{ $$ = $1; }
//	|	VAR							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//;
//
//method_modifiers:
//		/* empty */							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//	|	non_empty_member_modifiers			{ $$ = $1;  if (!(Z_LVAL($$.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL($$.u.constant) |= ZEND_ACC_PUBLIC; } }
//;
//
//non_empty_member_modifiers:
//		member_modifier						{ $$ = $1; }
//	|	non_empty_member_modifiers member_modifier	{ Z_LVAL($$.u.constant) = zend_do_verify_access_types(&$1, &$2); }
//;
//
//member_modifier:
//		PUBLIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//	|	PROTECTED				{ Z_LVAL($$.u.constant) = ZEND_ACC_PROTECTED; }
//	|	PRIVATE				{ Z_LVAL($$.u.constant) = ZEND_ACC_PRIVATE; }
//	|	STATIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_STATIC; }
//	|	ABSTRACT				{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
//	|	FINAL					{ Z_LVAL($$.u.constant) = ZEND_ACC_FINAL; }
//;
//
//class_variable_declaration:
//		class_variable_declaration COMMA VARIABLE					{ zend_do_declare_property(&$3, NULL, CG(access_type) TSRMLS_CC); }
//	|	class_variable_declaration COMMA VARIABLE EQUAL static_scalar	{ zend_do_declare_property(&$3, &$5, CG(access_type) TSRMLS_CC); }
//	|	VARIABLE						{ zend_do_declare_property(&$1, NULL, CG(access_type) TSRMLS_CC); }
//	|	VARIABLE EQUAL static_scalar	{ zend_do_declare_property(&$1, &$3, CG(access_type) TSRMLS_CC); }
//;
//
//class_constant_declaration:
//		class_constant_declaration COMMA STRING EQUAL static_scalar	{ zend_do_declare_class_constant(&$3, &$5 TSRMLS_CC); }
//	|	CONST STRING EQUAL static_scalar	{ zend_do_declare_class_constant(&$2, &$4 TSRMLS_CC); }
//;
//
//echo_expr_list:
//		echo_expr_list COMMA expr { zend_do_echo(&$3 TSRMLS_CC); }
//	|	expr					{ zend_do_echo(&$1 TSRMLS_CC); }
//;
//

echo_expr_list ::= expr(A). { zend_do_echo(&A TSRMLS_CC); }
echo_expr_list ::= echo_expr_list COMMA expr(A). { zend_do_echo(&A TSRMLS_CC); }

//
//for_expr:
//		/* empty */			{ $$.op_type = IS_CONST;  Z_TYPE($$.u.constant) = IS_BOOL;  Z_LVAL($$.u.constant) = 1; }
//	|	non_empty_for_expr	{ $$ = $1; }
//;
//
//non_empty_for_expr:
//		non_empty_for_expr COMMA	{ zend_do_free(&$1 TSRMLS_CC); } expr { $$ = $4; }
//	|	expr					{ $$ = $1; }
//;
//
//expr_without_variable:
//		LIST LPAREN { zend_do_list_init(TSRMLS_C); } assignment_list RPAREN EQUAL expr { zend_do_list_end(&$$, &$7 TSRMLS_CC); }
//	|	variable EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_assign(&$$, &$1, &$3 TSRMLS_CC); }
//	|	variable EQUAL '&' variable { zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$4, BP_VAR_W, 1 TSRMLS_CC); zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&$$, &$1, &$4 TSRMLS_CC); }
//	|	variable EQUAL '&' NEW class_name_reference { zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");  zend_check_writable_variable(&$1); zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$4, &$5 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$3, &$4, &$7 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $3.EA = ZEND_PARSED_NEW; zend_do_assign_ref(&$$, &$1, &$3 TSRMLS_CC); }
//	|	NEW class_name_reference { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$1, &$2 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$$, &$1, &$4 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//	|	CLONE expr { zend_do_clone(&$$, &$2 TSRMLS_CC); }
//	|	variable PLUS_EQUAL expr 	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_ADD, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable MINUS_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SUB, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable MUL_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MUL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable DIV_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_DIV, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable CONCAT_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_CONCAT, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable MOD_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MOD, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable AND_EQUAL expr		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_AND, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable OR_EQUAL expr 		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_OR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable XOR_EQUAL expr 		{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable SL_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	variable SR_EQUAL expr	{ zend_check_writable_variable(&$1); zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	rw_variable INC { zend_do_post_incdec(&$$, &$1, ZEND_POST_INC TSRMLS_CC); }
//	|	INC rw_variable { zend_do_pre_incdec(&$$, &$2, ZEND_PRE_INC TSRMLS_CC); }
//	|	rw_variable DEC { zend_do_post_incdec(&$$, &$1, ZEND_POST_DEC TSRMLS_CC); }
//	|	DEC rw_variable { zend_do_pre_incdec(&$$, &$2, ZEND_PRE_DEC TSRMLS_CC); }
//	|	expr BOOLEAN_OR { zend_do_boolean_or_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_or_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
//	|	expr BOOLEAN_AND { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_and_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
//	|	expr LOGICAL_OR { zend_do_boolean_or_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_or_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
//	|	expr LOGICAL_AND { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } expr { zend_do_boolean_and_end(&$$, &$1, &$4, &$2 TSRMLS_CC); }
//	|	expr LOGICAL_XOR expr { zend_do_binary_op(ZEND_BOOL_XOR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '|' expr	{ zend_do_binary_op(ZEND_BW_OR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '&' expr	{ zend_do_binary_op(ZEND_BW_AND, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '^' expr	{ zend_do_binary_op(ZEND_BW_XOR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '.' expr 	{ zend_do_binary_op(ZEND_CONCAT, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '+' expr 	{ zend_do_binary_op(ZEND_ADD, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '-' expr 	{ zend_do_binary_op(ZEND_SUB, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '*' expr	{ zend_do_binary_op(ZEND_MUL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '/' expr	{ zend_do_binary_op(ZEND_DIV, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '%' expr 	{ zend_do_binary_op(ZEND_MOD, &$$, &$1, &$3 TSRMLS_CC); }
//	| 	expr SL expr	{ zend_do_binary_op(ZEND_SL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr SR expr	{ zend_do_binary_op(ZEND_SR, &$$, &$1, &$3 TSRMLS_CC); }
//	|	'+' expr %prec INC { ZVAL_LONG(&$1.u.constant, 0); if ($2.op_type == IS_CONST) { add_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; } else { $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_ADD, &$$, &$1, &$2 TSRMLS_CC); } }
//	|	'-' expr %prec INC { ZVAL_LONG(&$1.u.constant, 0); if ($2.op_type == IS_CONST) { sub_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; } else { $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_SUB, &$$, &$1, &$2 TSRMLS_CC); } }
//	|	'!' expr { zend_do_unary_op(ZEND_BOOL_NOT, &$$, &$2 TSRMLS_CC); }
//	|	'~' expr { zend_do_unary_op(ZEND_BW_NOT, &$$, &$2 TSRMLS_CC); }
//	|	expr IS_IDENTICAL expr		{ zend_do_binary_op(ZEND_IS_IDENTICAL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr IS_NOT_IDENTICAL expr	{ zend_do_binary_op(ZEND_IS_NOT_IDENTICAL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr IS_EQUAL expr			{ zend_do_binary_op(ZEND_IS_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr IS_NOT_EQUAL expr 		{ zend_do_binary_op(ZEND_IS_NOT_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '<' expr 					{ zend_do_binary_op(ZEND_IS_SMALLER, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr IS_SMALLER_OR_EQUAL expr { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3 TSRMLS_CC); }
//	|	expr '>' expr 					{ zend_do_binary_op(ZEND_IS_SMALLER, &$$, &$3, &$1 TSRMLS_CC); }
//	|	expr IS_GREATER_OR_EQUAL expr { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1 TSRMLS_CC); }
//	|	expr INSTANCEOF class_name_reference { zend_do_instanceof(&$$, &$1, &$3, 0 TSRMLS_CC); }
//	|	LPAREN expr RPAREN 	{ $$ = $2; }
//	|	expr '?' { zend_do_begin_qm_op(&$1, &$2 TSRMLS_CC); }
//		expr COLON { zend_do_qm_true(&$4, &$2, &$5 TSRMLS_CC); }
//		expr	 { zend_do_qm_false(&$$, &$7, &$2, &$5 TSRMLS_CC); }
//	|	expr '?' COLON { zend_do_jmp_set(&$1, &$2, &$3 TSRMLS_CC); }
//		expr     { zend_do_jmp_set_else(&$$, &$5, &$2, &$3 TSRMLS_CC); }
//	|	internal_functions_in_yacc { $$ = $1; }
//	|	INT_CAST expr 	{ zend_do_cast(&$$, &$2, IS_LONG TSRMLS_CC); }
//	|	DOUBLE_CAST expr 	{ zend_do_cast(&$$, &$2, IS_DOUBLE TSRMLS_CC); }
//	|	STRING_CAST expr	{ zend_do_cast(&$$, &$2, IS_STRING TSRMLS_CC); }
//	|	ARRAY_CAST expr 	{ zend_do_cast(&$$, &$2, IS_ARRAY TSRMLS_CC); }
//	|	OBJECT_CAST expr 	{ zend_do_cast(&$$, &$2, IS_OBJECT TSRMLS_CC); }
//	|	BOOL_CAST expr	{ zend_do_cast(&$$, &$2, IS_BOOL TSRMLS_CC); }
//	|	SCALAR_CAST expr 	{ zend_do_cast(&$$, &$2, IS_SCALAR TSRMLS_CC); }
//	|	NUMERIC_CAST expr 	{ zend_do_cast(&$$, &$2, IS_NUMERIC TSRMLS_CC); }
//	|	UNSET_CAST expr	{ zend_do_cast(&$$, &$2, IS_NULL TSRMLS_CC); }
//	|	EXIT exit_expr	{ zend_do_exit(&$$, &$2 TSRMLS_CC); }
//	|	'@' { zend_do_begin_silence(&$1 TSRMLS_CC); } expr { zend_do_end_silence(&$1 TSRMLS_CC); $$ = $3; }
//	|	scalar				{ $$ = $1; }
//	|	ARRAY LPAREN array_pair_list RPAREN { $$ = $3; }
//	|	'`' backticks_expr '`' { zend_do_shell_exec(&$$, &$2 TSRMLS_CC); }
//	|	PRINT expr  { zend_do_print(&$$, &$2 TSRMLS_CC); }
//	|	function is_reference LPAREN { zend_do_begin_lambda_function_declaration(&$$, &$1, $2.op_type, 0 TSRMLS_CC); }
//			parameter_list RPAREN lexical_vars LBRACE inner_statement_list RBRACE {  zend_do_end_function_declaration(&$1 TSRMLS_CC); $$ = $4; }
//	|	STATIC function is_reference LPAREN { zend_do_begin_lambda_function_declaration(&$$, &$2, $3.op_type, 1 TSRMLS_CC); }
//			parameter_list RPAREN lexical_vars LBRACE inner_statement_list RBRACE {  zend_do_end_function_declaration(&$2 TSRMLS_CC); $$ = $5; }
//;
//

expr_without_variable(A) ::= scalar(B). { A = B; }


//function:
//	FUNCTION { $$.u.op.opline_num = CG(zend_lineno); }
//;
//
//lexical_vars:
//		/* empty */
//	|	USE LPAREN lexical_var_list RPAREN
//;
//
//lexical_var_list:
//		lexical_var_list COMMA VARIABLE			{ zend_do_fetch_lexical_variable(&$3, 0 TSRMLS_CC); }
//	|	lexical_var_list COMMA '&' VARIABLE		{ zend_do_fetch_lexical_variable(&$4, 1 TSRMLS_CC); }
//	|	VARIABLE								{ zend_do_fetch_lexical_variable(&$1, 0 TSRMLS_CC); }
//	|	'&' VARIABLE							{ zend_do_fetch_lexical_variable(&$2, 1 TSRMLS_CC); }
//;
//
//function_call:
//		namespace_name LPAREN { $2.u.op.opline_num = zend_do_begin_function_call(&$1, 1 TSRMLS_CC); }
//				function_call_parameter_list
//				RPAREN { zend_do_end_function_call(&$1, &$$, &$4, 0, $2.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
//	|	NAMESPACE NS_SEPARATOR namespace_name LPAREN { $1.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$1.u.constant);  zend_do_build_namespace_name(&$1, &$1, &$3 TSRMLS_CC); $4.u.op.opline_num = zend_do_begin_function_call(&$1, 0 TSRMLS_CC); }
//				function_call_parameter_list
//				RPAREN { zend_do_end_function_call(&$1, &$$, &$6, 0, $4.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
//	|	NS_SEPARATOR namespace_name LPAREN { $3.u.op.opline_num = zend_do_begin_function_call(&$2, 0 TSRMLS_CC); }
//				function_call_parameter_list
//				RPAREN { zend_do_end_function_call(&$2, &$$, &$5, 0, $3.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
//	|	class_name PAAMAYIM_NEKUDOTAYIM STRING LPAREN { $4.u.op.opline_num = zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
//			function_call_parameter_list
//			RPAREN { zend_do_end_function_call($4.u.op.opline_num?NULL:&$3, &$$, &$6, $4.u.op.opline_num, $4.u.op.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//	|	class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects LPAREN { zend_do_end_variable_parse(&$3, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
//			function_call_parameter_list
//			RPAREN { zend_do_end_function_call(NULL, &$$, &$6, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM STRING LPAREN { zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
//			function_call_parameter_list
//			RPAREN { zend_do_end_function_call(NULL, &$$, &$6, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects LPAREN { zend_do_end_variable_parse(&$3, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_class_member_function_call(&$1, &$3 TSRMLS_CC); }
//			function_call_parameter_list
//			RPAREN { zend_do_end_function_call(NULL, &$$, &$6, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//	|	variable_without_objects  LPAREN { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_dynamic_function_call(&$1, 0 TSRMLS_CC); }
//			function_call_parameter_list RPAREN
//			{ zend_do_end_function_call(&$1, &$$, &$4, 0, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
//;
//
//class_name:
//		STATIC { $$.op_type = IS_CONST; ZVAL_STRINGL(&$$.u.constant, "static", sizeof("static")-1, 1);}
//	|	namespace_name { $$ = $1; }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
//;
//
//fully_qualified_class_name:
//		namespace_name { $$ = $1; }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
//;
//
//
//
//class_name_reference:
//		class_name						{ zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
//	|	dynamic_class_name_reference	{ zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
//;
//
//
//dynamic_class_name_reference:
//		base_variable OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
//			object_property { zend_do_push_object(&$4 TSRMLS_CC); } dynamic_class_name_variable_properties
//			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = ZEND_PARSED_MEMBER; }
//	|	base_variable { $$ = $1; }
//;
//
//
//dynamic_class_name_variable_properties:
//		dynamic_class_name_variable_properties dynamic_class_name_variable_property
//	|	/* empty */
//;
//
//
//dynamic_class_name_variable_property:
//		OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); }
//;
//
//exit_expr:
//		/* empty */	{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
//	|	LPAREN RPAREN		{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
//	|	LPAREN expr RPAREN	{ $$ = $2; }
//;
//
//backticks_expr:
//		/* empty */	{ ZVAL_EMPTY_STRING(&$$.u.constant); INIT_PZVAL(&$$.u.constant); $$.op_type = IS_CONST; }
//	|	ENCAPSED_AND_WHITESPACE	{ $$ = $1; }
//	|	encaps_list	{ $$ = $1; }
//;
//
//
//ctor_arguments:
//		/* empty */	{ Z_LVAL($$.u.constant)=0; }
//	|	LPAREN function_call_parameter_list RPAREN	{ $$ = $2; }
//;
//
//

common_scalar(A) ::= LNUMBER(B). 					{ A = B; }
common_scalar(A) ::= DNUMBER(B). 					{ A = B; }
common_scalar(A) ::= CONSTANT_ENCAPSED_STRING(B).	{ A = B; }
common_scalar(A) ::= LINE(B). 						{ A = B; }
common_scalar(A) ::= FILE(B). 						{ A = B; }
common_scalar(A) ::= DIR(B).   						{ A = B; }
common_scalar(A) ::= CLASS_C(B).						{ A = B; }
common_scalar(A) ::= METHOD_C(B).					{ A = B; }
common_scalar(A) ::= FUNC_C(B).						{ A = B; }
common_scalar(A) ::= NS_C(B).						{ A = B; }
//common_scalar(A) ::= START_HEREDOC ENCAPSED_AND_WHITESPACE END_HEREDOC { $$ = $2; CG(heredoc) = Z_STRVAL($1.u.constant); CG(heredoc_len) = Z_STRLEN($1.u.constant); }
//common_scalar(A) ::= START_HEREDOC END_HEREDOC { ZVAL_EMPTY_STRING(&$$.u.constant); INIT_PZVAL(&$$.u.constant); $$.op_type = IS_CONST; CG(heredoc) = Z_STRVAL($1.u.constant); CG(heredoc_len) = Z_STRLEN($1.u.constant); }
//
//
//
//static_scalar: /* compile-time evaluated scalars */
//		common_scalar		{ $$ = $1; }
//	|	namespace_name 		{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_CT, 1 TSRMLS_CC); }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_CT, 0 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); zend_do_fetch_constant(&$$, NULL, &$2, ZEND_CT, 0 TSRMLS_CC); }
//	|	'+' static_scalar { ZVAL_LONG(&$1.u.constant, 0); add_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; }
//	|	'-' static_scalar { ZVAL_LONG(&$1.u.constant, 0); sub_function(&$2.u.constant, &$1.u.constant, &$2.u.constant TSRMLS_CC); $$ = $2; }
//	|	ARRAY LPAREN static_array_pair_list RPAREN { $$ = $3; Z_TYPE($$.u.constant) = IS_CONSTANT_ARRAY; }
//	|	static_class_constant { $$ = $1; }
//;

static_scalar(A) ::= common_scalar(B). { A = B; }

//
//static_class_constant:
//		class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_CT, 0 TSRMLS_CC); }
//;
//
//scalar:
//		STRING_VARNAME		{ $$ = $1; }
//	|	class_constant		{ $$ = $1; }
//	|	namespace_name	{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_RT, 1 TSRMLS_CC); }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); $3 = $$; zend_do_fetch_constant(&$$, NULL, &$3, ZEND_RT, 0 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); zend_do_fetch_constant(&$$, NULL, &$2, ZEND_RT, 0 TSRMLS_CC); }
//	|	common_scalar			{ $$ = $1; }
//	|	'"' encaps_list '"' 	{ $$ = $2; }
//	|	START_HEREDOC encaps_list END_HEREDOC { $$ = $2; CG(heredoc) = Z_STRVAL($1.u.constant); CG(heredoc_len) = Z_STRLEN($1.u.constant); }
//;
//

scalar(A) ::= common_scalar(B). { A = B; }
//
//static_array_pair_list:
//		/* empty */ { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); }
//	|	non_empty_static_array_pair_list possible_comma	{ $$ = $1; }
//;
//
//possible_comma:
//		/* empty */
//	|	COMMA
//;
//
//non_empty_static_array_pair_list:
//		non_empty_static_array_pair_list COMMA static_scalar DOUBLE_ARROW static_scalar	{ zend_do_add_static_array_element(&$$, &$3, &$5); }
//	|	non_empty_static_array_pair_list COMMA static_scalar { zend_do_add_static_array_element(&$$, NULL, &$3); }
//	|	static_scalar DOUBLE_ARROW static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, &$1, &$3); }
//	|	static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, NULL, &$1); }
//;
//
//expr:
//		r_variable					{ $$ = $1; }
//	|	expr_without_variable		{ $$ = $1; }
//;
//

expr(R) ::= expr_without_variable(A).	{ R = A; }

//
//r_variable:
//	variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$ = $1; }
//;
//
//
//w_variable:
//	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $$ = $1;
//				  zend_check_writable_variable(&$1); }
//;
//
//rw_variable:
//	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); $$ = $1;
//				  zend_check_writable_variable(&$1); }
//;
//
//variable:
//		base_variable_with_function_calls OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
//			object_property { zend_do_push_object(&$4 TSRMLS_CC); } method_or_not variable_properties
//			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = $1.EA | ($7.EA ? $7.EA : $6.EA); }
//	|	base_variable_with_function_calls { $$ = $1; }
//;
//
//variable_properties:
//		variable_properties variable_property { $$.EA = $2.EA; }
//	|	/* empty */ { $$.EA = 0; }
//;
//
//
//variable_property:
//		OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); } method_or_not { $$.EA = $4.EA; }
//;
//
//array_method_dereference:
//		array_method_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	method '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//;
//
//method:
//		LPAREN { zend_do_pop_object(&$1 TSRMLS_CC); zend_do_begin_method_call(&$1 TSRMLS_CC); }
//				function_call_parameter_list RPAREN
//			{ zend_do_end_function_call(&$1, &$$, &$3, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
//;
//
//method_or_not:			
//		method						{ $$ = $1; zend_do_push_object(&$$ TSRMLS_CC); $$.EA = ZEND_PARSED_METHOD_CALL; }
//	|	array_method_dereference	{ $$ = $1; zend_do_push_object(&$$ TSRMLS_CC); }
//	|	/* empty */ { $$.EA = ZEND_PARSED_MEMBER; }
//;
//
//variable_without_objects:
//		reference_variable { $$ = $1; }
//	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); }
//;
//
//static_member:
//		class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }
//
//;
//
//variable_class_name:
//		reference_variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$=$1;; }
//;
//
//array_function_dereference:
//		array_function_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$.EA = ZEND_PARSED_FUNCTION_CALL; } 
//		'[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$4 TSRMLS_CC); }
//;
//
//base_variable_with_function_calls:
//		base_variable				{ $$ = $1; }
//	|	array_function_dereference	{ $$ = $1; }
//	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$ = $1; $$.EA = ZEND_PARSED_FUNCTION_CALL; }
//;
//
//
//base_variable:
//		reference_variable { $$ = $1; $$.EA = ZEND_PARSED_VARIABLE; }
//	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); $$.EA = ZEND_PARSED_VARIABLE; }
//	|	static_member { $$ = $1; $$.EA = ZEND_PARSED_STATIC_MEMBER; }
//;
//
//reference_variable:
//		reference_variable '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	reference_variable LBRACE expr RBRACE		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
//	|	compound_variable			{ zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//;
//
//
//compound_variable:
//		VARIABLE			{ $$ = $1; }
//	|	'$' LBRACE expr RBRACE	{ $$ = $3; }
//;
//
//dim_offset:
//		/* empty */		{ $$.op_type = IS_UNUSED; }
//	|	expr			{ $$ = $1; }
//;
//
//
//object_property:
//		object_dim_list { $$ = $1; }
//	|	variable_without_objects { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); } { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
//;
//
//object_dim_list:
//		object_dim_list '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	object_dim_list LBRACE expr RBRACE		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
//	|	variable_name { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
//;
//
//variable_name:
//		STRING		{ $$ = $1; }
//	|	LBRACE expr RBRACE	{ $$ = $2; }
//;
//
//simple_indirect_reference:
//		'$' { Z_LVAL($$.u.constant) = 1; }
//	|	simple_indirect_reference '$' { Z_LVAL($$.u.constant)++; }
//;
//
//assignment_list:
//		assignment_list COMMA assignment_list_element
//	|	assignment_list_element
//;
//
//
//assignment_list_element:
//		variable								{ zend_do_add_list_element(&$1 TSRMLS_CC); }
//	|	LIST LPAREN { zend_do_new_list_begin(TSRMLS_C); } assignment_list RPAREN	{ zend_do_new_list_end(TSRMLS_C); }
//	|	/* empty */							{ zend_do_add_list_element(NULL TSRMLS_CC); }
//;
//
//
//array_pair_list:
//		/* empty */ { zend_do_init_array(&$$, NULL, NULL, 0 TSRMLS_CC); }
//	|	non_empty_array_pair_list possible_comma	{ $$ = $1; }
//;
//
//non_empty_array_pair_list:
//		non_empty_array_pair_list COMMA expr DOUBLE_ARROW expr	{ zend_do_add_array_element(&$$, &$5, &$3, 0 TSRMLS_CC); }
//	|	non_empty_array_pair_list COMMA expr			{ zend_do_add_array_element(&$$, &$3, NULL, 0 TSRMLS_CC); }
//	|	expr DOUBLE_ARROW expr	{ zend_do_init_array(&$$, &$3, &$1, 0 TSRMLS_CC); }
//	|	expr 				{ zend_do_init_array(&$$, &$1, NULL, 0 TSRMLS_CC); }
//	|	non_empty_array_pair_list COMMA expr DOUBLE_ARROW '&' w_variable { zend_do_add_array_element(&$$, &$6, &$3, 1 TSRMLS_CC); }
//	|	non_empty_array_pair_list COMMA '&' w_variable { zend_do_add_array_element(&$$, &$4, NULL, 1 TSRMLS_CC); }
//	|	expr DOUBLE_ARROW '&' w_variable	{ zend_do_init_array(&$$, &$4, &$1, 1 TSRMLS_CC); }
//	|	'&' w_variable 			{ zend_do_init_array(&$$, &$2, NULL, 1 TSRMLS_CC); }
//;
//
//encaps_list:
//		encaps_list encaps_var { zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&$$, &$1, &$2 TSRMLS_CC); }
//	|	encaps_list ENCAPSED_AND_WHITESPACE	{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
//	|	encaps_var { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, NULL, &$1 TSRMLS_CC); }
//	|	ENCAPSED_AND_WHITESPACE encaps_var	{ zend_do_add_string(&$$, NULL, &$1 TSRMLS_CC); zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, &$$, &$2 TSRMLS_CC); }
//;
//
//
//
//encaps_var:
//		VARIABLE { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//	|	VARIABLE '[' { zend_do_begin_variable_parse(TSRMLS_C); } encaps_var_offset ']'	{ fetch_array_begin(&$$, &$1, &$4 TSRMLS_CC); }
//	|	VARIABLE OBJECT_OPERATOR STRING { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$2, &$1, 1 TSRMLS_CC); zend_do_fetch_property(&$$, &$2, &$3 TSRMLS_CC); }
//	|	DOLLAR_OPEN_CURLY_BRACES expr RBRACE { zend_do_begin_variable_parse(TSRMLS_C);  fetch_simple_variable(&$$, &$2, 1 TSRMLS_CC); }
//	|	DOLLAR_OPEN_CURLY_BRACES STRING_VARNAME '[' expr ']' RBRACE { zend_do_begin_variable_parse(TSRMLS_C);  fetch_array_begin(&$$, &$2, &$4 TSRMLS_CC); }
//	|	CURLY_OPEN variable RBRACE { $$ = $2; }
//;
//
//
//encaps_var_offset:
//		STRING		{ $$ = $1; }
//	|	NUM_STRING	{ $$ = $1; }
//	|	VARIABLE		{ fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//;
//
//
//internal_functions_in_yacc:
//		ISSET LPAREN isset_variables RPAREN { $$ = $3; }
//	|	EMPTY LPAREN variable RPAREN	{ zend_do_isset_or_isempty(ZEND_ISEMPTY, &$$, &$3 TSRMLS_CC); }
//	|	INCLUDE expr 			{ zend_do_include_or_eval(ZEND_INCLUDE, &$$, &$2 TSRMLS_CC); }
//	|	INCLUDE_ONCE expr 	{ zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &$$, &$2 TSRMLS_CC); }
//	|	EVAL LPAREN expr RPAREN 	{ zend_do_include_or_eval(ZEND_EVAL, &$$, &$3 TSRMLS_CC); }
//	|	REQUIRE expr			{ zend_do_include_or_eval(ZEND_REQUIRE, &$$, &$2 TSRMLS_CC); }
//	|	REQUIRE_ONCE expr		{ zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &$$, &$2 TSRMLS_CC); }
//;
//
//isset_variables:
//		variable 				{ zend_do_isset_or_isempty(ZEND_ISSET, &$$, &$1 TSRMLS_CC); }
//	|	isset_variables COMMA { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } variable { znode tmp; zend_do_isset_or_isempty(ZEND_ISSET, &tmp, &$4 TSRMLS_CC); zend_do_boolean_and_end(&$$, &$1, &tmp, &$2 TSRMLS_CC); }
//;
//
//class_constant:
//		class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
//;

// Just to the build works
todo ::= OPEN_TAG OPEN_TAG_WITH_ECHO ENCAPSED_AND_WHITESPACE CURLY_OPEN VARIABLE
	DOLLAR_OPEN_CURLY_BRACES END_HEREDOC WHITESPACE COMMENT CLOSE_TAG DOC_COMMENT
	START_HEREDOC OBJECT_HINT INT_HINT LIST DOUBLE_ARROW UNSET VAR DEFAULT
	DECLARE EMPTY STRING_VARNAME NUM_STRING ENDFOREACH ENDDECLARE EXTENDS ENDWHILE
	DO FOREACH FOR FUNCTION CATCH CASE CLASS ENDSWITCH ENDFOR IMPLEMENTS INTERFACE
	TRY TRAIT INSTEADOF OBJECT_OPERATOR GLOBAL STRING_HINT SWITCH SCALAR_HINT ARRAY
	ISSET RESOURCE_HINT DOUBLE_HINT BOOL_HINT PAAMAYIM_NEKUDOTAYIM NUMERIC_HINT.


//
//%%
//
///*
// * Local variables:
// * tab-width: 4
// * c-basic-offset: 4
// * indent-tabs-mode: t
// * End:
// */
