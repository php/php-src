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
    zend_bailout();
}

%stack_overflow {
    fprintf(stderr,"Woops, parser stack overflow\n");
}

%stack_size 500

%start_symbol start

%token_prefix T_

%token_type {znode}

%ifdef ZTS
	%extra_argument {void ***tsrm_ls}
%endif

%type function_call_ns_i {znode_array}
%type function_call_ns_ii {znode_array}
%type function_call_ns_iii {znode_array}
%type function_call_cls_i {znode_array}
%type additional_catch_i {znode_array}
%type class_statement_ii {znode_array}
%type new_ii {znode_array}
%type foreach_i {znode_array}
%type foreach_ii {znode_array}
%type foreach2_i {znode_array}
%type foreach2_ii {znode_array}
%type bool_or_expr_i {znode_array}
%type bool_and_expr_i {znode_array}
%type logic_or_expr_i {znode_array}
%type logic_and_expr_i {znode_array}
%type ternary_i {znode_array}
%type short_ternary_i {znode_array}
%type closure_i {znode_array}
%type closure_ii {znode_array}
%type try_catch_iv {znode_array}
%type try_catch_iii {znode_array}
%type isset_variables_i {znode_array}
%type unticked_class_declaration_statement_i {znode_array}
%type unticked_class_declaration_statement_ii {znode_array}
%type while_cond {znode_array}
%type do_statement {znode_array}
%type for_cont {znode_array}


/* TOKENS TRANSLATION:
   , => T_COMMA
   = => T_EQUAL
   ( => T_LPAREN
   ) => T_LPAREN
   [ => T_LBRACKET
   ] => T_RBRACKET
   ; => T_SEMICOLON
   : => T_COLON
   { => T_LBRACE
   } => T_RBRACE
   + => T_PLUS
   - => T_MINUS
   % => T_MOD
   / => T_DIV
   * => T_MULT
   & => T_BW_AND
   | => T_BW_OR
   ^ => T_BW_XOR
   @ => T_AT
   " => T_QUOTE
   ? => T_QUESTION_MARK
   ! => T_BOOL_NOT
   > => T_GREATER
   < => T_SMALLER
   ` => T_BACKQUOTE

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
%left BW_OR.
%left BW_XOR.
%left BW_AND.
%nonassoc IS_EQUAL IS_NOT_EQUAL IS_IDENTICAL IS_NOT_IDENTICAL.
%nonassoc IS_SMALLER IS_SMALLER_OR_EQUAL IS_GREATER IS_GREATER_OR_EQUAL.
%left SL SR.
%left PLUS MINUS DOT.
%left MULT DIV MOD.
%right BOOL_NOT.
%nonassoc INSTANCEOF.
%right BW_NOT INC DEC INT_CAST NUMERIC_CAST SCALAR_CAST DOUBLE_CAST STRING_CAST ARRAY_CAST OBJECT_CAST BOOL_CAST UNSET_CAST AT.
%right RBRACKET.
%nonassoc NEW CLONE.
%nonassoc EXIT.
%nonassoc IF.
%left ELSEIF.
%left ELSE.
%left ENDIF.

%right STATIC ABSTRACT FINAL PRIVATE PROTECTED PUBLIC.

// dummy tokens
%nonassoc OPEN_TAG OPEN_TAG_WITH_ECHO WHITESPACE COMMENT CLOSE_TAG DOC_COMMENT.


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

//namespace_name:
//		STRING { $$ = $1; }
//	|	namespace_name NS_SEPARATOR STRING { zend_do_build_namespace_name(&$$, &$1, &$3 TSRMLS_CC); }
//;

namespace_name(A) ::= STRING(B). { A = B; }
namespace_name(A) ::= namespace_name(B) NS_SEPARATOR STRING(C). { zend_do_build_namespace_name(&A, &B, &C TSRMLS_CC); }

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

namespace_start ::= NAMESPACE LBRACE. { zend_do_begin_namespace(NULL, 1 TSRMLS_CC); }

top_statement ::= statement.                                       { zend_verify_namespace(TSRMLS_C); }
top_statement ::= function_declaration_statement.                  { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
top_statement ::= class_declaration_statement.                     { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
top_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON.   { zend_do_halt_compiler_register(TSRMLS_C); /* YYACCEPT; */ }
top_statement ::= NAMESPACE namespace_name(B) SEMICOLON.       { zend_do_begin_namespace(&B, 0 TSRMLS_CC); }
top_statement ::= NAMESPACE namespace_name(B) LBRACE.          { zend_do_begin_namespace(&B, 1 TSRMLS_CC); }
top_statement ::= top_statement_list RBRACE.                     { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= namespace_start top_statement_list RBRACE.  { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= USE use_declarations SEMICOLON.              { zend_verify_namespace(TSRMLS_C); }
top_statement ::= constant_declaration SEMICOLON.                { zend_verify_namespace(TSRMLS_C); }

//use_declarations:
//		use_declarations COMMA use_declaration
//	|	use_declaration
//;

use_declarations ::= use_declarations COMMA use_declaration.
use_declarations ::= use_declaration.

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

//constant_declaration:
//		constant_declaration COMMA STRING EQUAL static_scalar	{ zend_do_declare_constant(&$3, &$5 TSRMLS_CC); }
//	|	CONST STRING EQUAL static_scalar { zend_do_declare_constant(&$2, &$4 TSRMLS_CC); }
//;

constant_declaration ::= constant_declaration COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_constant(&B, &C TSRMLS_CC); }
constant_declaration ::= CONST STRING(B) EQUAL static_scalar(C).                      { zend_do_declare_constant(&B, &C TSRMLS_CC); }

//inner_statement_list:
//		inner_statement_list  { zend_do_extended_info(TSRMLS_C); } inner_statement { HANDLE_INTERACTIVE(); }
//	|	/* empty */
//;

inner_statement_list ::= inner_statement_list_i inner_statement. { HANDLE_INTERACTIVE(); }
inner_statement_list ::= .

inner_statement_list_i ::= inner_statement_list. { zend_do_extended_info(TSRMLS_C); }

//inner_statement:
//		statement
//	|	function_declaration_statement
//	|	class_declaration_statement
//	|	HALT_COMPILER LPAREN RPAREN SEMICOLON   { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }
//;

inner_statement ::= statement.
inner_statement ::= function_declaration_statement.
inner_statement ::= class_declaration_statement.
inner_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON. { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }

//statement:
//		unticked_statement { zend_do_ticks(TSRMLS_C); }
//	|	STRING COLON { zend_do_label(&$1 TSRMLS_CC); }
//;

statement ::= unticked_statement.  { zend_do_ticks(TSRMLS_C); }
statement ::= STRING(B) COLON. { zend_do_label(&B TSRMLS_CC); }

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
if_cond_then ::= if_cond(B) statement.        { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

if_alt_cond(A)   ::= IF LPAREN expr(B) RPAREN(C) COLON. { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }
if_alt_cond_then ::= if_alt_cond(B) inner_statement_list. { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

while_begin(A) ::= WHILE(B) LPAREN. { B.u.op.opline_num = get_next_op_number(CG(active_op_array)); A = B; }
while_cond(A)  ::= while_begin(B)  expr(C) RPAREN(D). { zend_do_while_cond(&C, &D TSRMLS_CC); A[0] = B; A[1] = D; }
unticked_statement ::= while_cond(B) while_statement. { zend_do_while_end(&B[0], &B[1] TSRMLS_CC); }


do_start(A) ::= DO. { A.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); }
do_statement(A) ::= do_start(B) statement WHILE LPAREN(C). { C.u.op.opline_num = get_next_op_number(CG(active_op_array)); A[0] = B; A[1] = C; }
unticked_statement ::= do_statement(B) expr(C) RPAREN SEMICOLON. { zend_do_do_while_end(&B[0], &B[1], &C TSRMLS_CC); }

for_begin(A) ::= FOR LPAREN for_expr(B) SEMICOLON(C). { zend_do_free(&B TSRMLS_CC); C.u.op.opline_num = get_next_op_number(CG(active_op_array)); A = C; }
for_cont(A) ::= for_begin(B) for_expr(C) SEMICOLON(D). { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&C, &D TSRMLS_CC); A[0] = B; A[1] = D; }
for_cont_2(A) ::= for_cont(B) for_expr(C) RPAREN. { zend_do_free(&C TSRMLS_CC); zend_do_for_before_statement(&B[0], &B[1] TSRMLS_CC); A = B[1]; }

unticked_statement ::= for_cont_2(B) for_statement. { zend_do_for_end(&B TSRMLS_CC); }

switch_i ::=  SWITCH LPAREN expr(B) RPAREN. { zend_do_switch_cond(&B TSRMLS_CC); }

unticked_statement ::= switch_i switch_case_list(B). { zend_do_switch_end(&B TSRMLS_CC); }

unticked_statement ::= LBRACE inner_statement_list RBRACE.
unticked_statement ::= if_cond_then elseif_list else_single. { zend_do_if_end(TSRMLS_C); }
unticked_statement ::= if_alt_cond_then elseif_alt_list else_alt_single ENDIF SEMICOLON. { zend_do_if_end(TSRMLS_C); }
unticked_statement ::= BREAK SEMICOLON.            { zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
unticked_statement ::= BREAK expr(B) SEMICOLON.    { zend_do_brk_cont(ZEND_BRK, &B TSRMLS_CC); }
unticked_statement ::= CONTINUE SEMICOLON.         { zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
unticked_statement ::= CONTINUE expr(B) SEMICOLON. { zend_do_brk_cont(ZEND_CONT, &B TSRMLS_CC); }
unticked_statement ::= RETURN SEMICOLON.                            { zend_do_return(NULL, 0 TSRMLS_CC); }
unticked_statement ::= RETURN expr_without_variable(B) SEMICOLON.   { zend_do_return(&B, 0 TSRMLS_CC); }
unticked_statement ::= RETURN variable(B) SEMICOLON.                { zend_do_return(&B, 1 TSRMLS_CC); }
unticked_statement ::= GLOBAL global_var_list SEMICOLON.
unticked_statement ::= STATIC static_var_list SEMICOLON.
unticked_statement ::= ECHO echo_expr_list SEMICOLON.
unticked_statement ::= INLINE_HTML(B). { zend_do_echo(&B TSRMLS_CC); }
unticked_statement ::= expr(B) SEMICOLON. { zend_do_free(&B TSRMLS_CC); }
unticked_statement ::= UNSET LPAREN unset_variables RPAREN SEMICOLON.
unticked_statement ::= foreach.
unticked_statement ::= foreach2.
unticked_statement ::= declare.
unticked_statement ::= SEMICOLON. /* empty statement */
unticked_statement ::= try_catch.
unticked_statement ::= THROW expr(B) SEMICOLON. { zend_do_throw(&B TSRMLS_CC); }
unticked_statement ::= GOTO STRING(B) SEMICOLON. { zend_do_goto(&B TSRMLS_CC); }

declare_i(A) ::= DECLARE(B). { A.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); A = B; }
declare ::= declare_i(B) LPAREN declare_list RPAREN declare_statement. { zend_do_declare_end(&B TSRMLS_CC); }

foreach_ii(A) ::= FOREACH(B) LPAREN(C) variable(D) AS(E). { zend_do_foreach_begin(&B, &C, &D, &E, 1 TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }
foreach_i(A)  ::= foreach_ii(B) foreach_variable(C) foreach_optional_arg(D) RPAREN. { zend_do_foreach_cont(&B[0], &B[1], &B[2], &C, &D TSRMLS_CC); A[0] = B[0]; A[1] = B[2]; }
foreach       ::= foreach_i(B) foreach_statement. { zend_do_foreach_end(&B[0], &B[1] TSRMLS_CC); }

foreach2_ii(A) ::= FOREACH(B) LPAREN(C) expr_without_variable(D) AS(E). { zend_do_foreach_begin(&B, &C, &D, &E, 0 TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }
foreach2_i(A)  ::= foreach2_ii(B) variable(C) foreach_optional_arg(D) RPAREN. { zend_check_writable_variable(&C); zend_do_foreach_cont(&B[0], &B[1], &B[2], &C, &D TSRMLS_CC); A[0] = B[0]; A[1] = B[2]; }
foreach2       ::= foreach2_i(B) foreach_statement. { zend_do_foreach_end(&B[0], &B[1] TSRMLS_CC); }

try_catch_v(A)   ::= TRY(B). { zend_do_try(&B TSRMLS_CC); A = B; }
try_catch_iv(A)  ::= try_catch_v(B) LBRACE inner_statement_list RBRACE CATCH LPAREN(C). { zend_initialize_try_catch_element(&B TSRMLS_CC); A[0] = B; A[1] = C; }
try_catch_iii(A) ::= try_catch_iv(B) fully_qualified_class_name(C). { zend_do_first_catch(&B[1] TSRMLS_CC); A[0] = B[0]; A[1] = B[1]; A[2] = C; }
try_catch_ii(A)  ::= try_catch_iii(B) VARIABLE(C) RPAREN. { zend_do_begin_catch(&B[0], &B[2], &C, &B[1] TSRMLS_CC); A = B[0]; }
try_catch_i(A)   ::= try_catch_ii(B) LBRACE inner_statement_list RBRACE. { zend_do_end_catch(&B TSRMLS_CC); A = B; }
try_catch        ::= try_catch_i(B) additional_catches(C). { zend_do_mark_last_catch(&B, &C TSRMLS_CC); }

//additional_catches:
//		non_empty_additional_catches { $$ = $1; }
//	|	/* empty */ { $$.u.op.opline_num = -1; }
//;

additional_catches(A) ::= . { A.u.op.opline_num = -1; }
additional_catches(A) ::= non_empty_additional_catches(B). { A = B; }

//non_empty_additional_catches:
//		additional_catch { $$ = $1; }
//	|	non_empty_additional_catches additional_catch { $$ = $2; }
//;

non_empty_additional_catches(A) ::= additional_catch(B). { A = B; }
non_empty_additional_catches(A) ::= non_empty_additional_catches additional_catch(B). { A = B; }

//additional_catch:
//	CATCH LPAREN fully_qualified_class_name { $$.u.op.opline_num = get_next_op_number(CG(active_op_array)); } VARIABLE RPAREN { zend_do_begin_catch(&$1, &$3, &$5, NULL TSRMLS_CC); } LBRACE inner_statement_list RBRACE { zend_do_end_catch(&$1 TSRMLS_CC); }
//;

additional_catch_i(A) ::= CATCH(B) LPAREN fully_qualified_class_name(C). { A[0] = B; A[1] = C; A[0].u.op.opline_num = get_next_op_number(CG(active_op_array)); }
additional_catch_ii(A) ::= additional_catch_i(B) VARIABLE(C) RPAREN. { zend_do_begin_catch(&B[0], &B[1], &C, NULL TSRMLS_CC); A = B[0]; }
additional_catch ::= additional_catch_ii(B) LBRACE inner_statement_list RBRACE. { zend_do_end_catch(&B TSRMLS_CC); }

//unset_variables:
//		unset_variable
//	|	unset_variables COMMA unset_variable
//;

unset_variables ::= unset_variable.
unset_variables ::= unset_variables COMMA unset_variable.

//unset_variable:
//		variable	{ zend_do_end_variable_parse(&$1, BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&$1 TSRMLS_CC); }
//;

unset_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&B TSRMLS_CC); A = B; }

//function_declaration_statement:
//		unticked_function_declaration_statement	{ zend_do_ticks(TSRMLS_C); }
//;

function_declaration_statement ::= unticked_function_declaration_statement. { zend_do_ticks(TSRMLS_C); }

//class_declaration_statement:
//		unticked_class_declaration_statement	{ zend_do_ticks(TSRMLS_C); }
//;

class_declaration_statement ::= unticked_class_declaration_statement. { zend_do_ticks(TSRMLS_C); }

//is_reference:
//		/* empty */	{ $$.op_type = ZEND_RETURN_VAL; }
//	|	'&'			{ $$.op_type = ZEND_RETURN_REF; }
//;

is_reference(A) ::= . { A.op_type = ZEND_RETURN_VAL; }
is_reference(A) ::= BW_AND. { A.op_type = ZEND_RETURN_REF; }

//unticked_function_declaration_statement:
//		function is_reference STRING { zend_do_begin_function_declaration(&$1, &$3, 0, $2.op_type, NULL TSRMLS_CC); }
//			LPAREN parameter_list RPAREN LBRACE inner_statement_list RBRACE { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
//;

unticked_function_declaration_statement_i(A) ::= function(B) is_reference(C) STRING(D). { zend_do_begin_function_declaration(&B, &D, 0, C.op_type, NULL TSRMLS_CC); A = B; }
unticked_function_declaration_statement ::= unticked_function_declaration_statement_i(B) LPAREN parameter_list RPAREN LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B TSRMLS_CC); }

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

unticked_class_declaration_statement_i(A) ::= class_entry_type(B) STRING(C) extends_from(D). { zend_do_begin_class_declaration(&B, &C, &D TSRMLS_CC); A[0] = B; A[1] = C; }
unticked_class_declaration_statement_ii(A) ::= interface_entry(B) STRING(C). { zend_do_begin_class_declaration(&B, &C, NULL TSRMLS_CC); A[0] = B; A[1] = C; }

unticked_class_declaration_statement ::= unticked_class_declaration_statement_i(B)
			implements_list
			LBRACE
				class_statement_list
			RBRACE. { zend_do_end_class_declaration(&B[0], &B[1] TSRMLS_CC); }
unticked_class_declaration_statement ::= unticked_class_declaration_statement_ii(B)
			interface_extends_list
			LBRACE
				class_statement_list
			RBRACE. { zend_do_end_class_declaration(&B[0], &B[1] TSRMLS_CC); }

//class_entry_type:
//		CLASS			{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = 0; }
//	|	ABSTRACT CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
//	|	TRAIT { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_TRAIT; }
//	|	FINAL CLASS { $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_FINAL_CLASS; }
//;

class_entry_type(A) ::= CLASS.			{ A.u.op.opline_num = CG(zend_lineno); A.EA = 0; }
class_entry_type(A) ::= ABSTRACT CLASS. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
class_entry_type(A) ::= TRAIT. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_TRAIT; }
class_entry_type(A) ::= FINAL CLASS. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_FINAL_CLASS; }

//extends_from:
//		/* empty */					{ $$.op_type = IS_UNUSED; }
//	|	EXTENDS fully_qualified_class_name	{ zend_do_fetch_class(&$$, &$2 TSRMLS_CC); }
//;

extends_from(A) ::= . { A.op_type = IS_UNUSED; }
extends_from(A) ::= EXTENDS fully_qualified_class_name(B). { zend_do_fetch_class(&A, &B TSRMLS_CC); }

//interface_entry:
//	INTERFACE		{ $$.u.op.opline_num = CG(zend_lineno); $$.EA = ZEND_ACC_INTERFACE; }
//;

interface_entry(A) ::= INTERFACE. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_INTERFACE; }

//interface_extends_list:
//		/* empty */
//	|	EXTENDS interface_list
//;

interface_extends_list ::= .
interface_extends_list ::= EXTENDS interface_list.

//implements_list:
//		/* empty */
//	|	IMPLEMENTS interface_list
//;

implements_list ::= .
implements_list ::= IMPLEMENTS interface_list.

//interface_list:
//		fully_qualified_class_name			{ zend_do_implements_interface(&$1 TSRMLS_CC); }
//	|	interface_list COMMA fully_qualified_class_name { zend_do_implements_interface(&$3 TSRMLS_CC); }
//;

interface_list ::= fully_qualified_class_name(B). { zend_do_implements_interface(&B TSRMLS_CC); }
interface_list ::= interface_list COMMA fully_qualified_class_name(B). { zend_do_implements_interface(&B TSRMLS_CC); }

//foreach_optional_arg:
//		/* empty */						{ $$.op_type = IS_UNUSED; }
//	|	DOUBLE_ARROW foreach_variable	{ $$ = $2; }
//;

foreach_optional_arg(A) ::= . { A.op_type = IS_UNUSED; }
foreach_optional_arg(A) ::= DOUBLE_ARROW foreach_variable(B). { A = B; }

//foreach_variable:
//		variable			{ zend_check_writable_variable(&$1); $$ = $1; }
//	|	'&' variable		{ zend_check_writable_variable(&$2); $$ = $2;  $$.EA |= ZEND_PARSED_REFERENCE_VARIABLE; }
//;

foreach_variable(A) ::= variable(B). { zend_check_writable_variable(&B); A = B; }
foreach_variable(A) ::= BW_AND variable(B). { zend_check_writable_variable(&B); A = B; A.EA |= ZEND_PARSED_REFERENCE_VARIABLE; }

//for_statement:
//		statement
//	|	COLON inner_statement_list ENDFOR SEMICOLON
//;

for_statement ::= statement.
for_statement ::= COLON inner_statement_list ENDFOR SEMICOLON.

//foreach_statement:
//		statement
//	|	COLON inner_statement_list ENDFOREACH SEMICOLON
//;

foreach_statement ::= statement.
foreach_statement ::= COLON inner_statement_list ENDFOREACH SEMICOLON.

//declare_statement:
//		statement
//	|	COLON inner_statement_list ENDDECLARE SEMICOLON
//;

declare_statement ::= statement.
declare_statement ::= COLON inner_statement_list ENDDECLARE SEMICOLON.

//declare_list:
//		STRING EQUAL static_scalar					{ zend_do_declare_stmt(&$1, &$3 TSRMLS_CC); }
//	|	declare_list COMMA STRING EQUAL static_scalar	{ zend_do_declare_stmt(&$3, &$5 TSRMLS_CC); }
//;

declare_list ::= STRING(B) EQUAL static_scalar(C).					{ zend_do_declare_stmt(&B, &C TSRMLS_CC); }
declare_list ::= declare_list COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_stmt(&B, &C TSRMLS_CC); }

//switch_case_list:
//		LBRACE case_list RBRACE					{ $$ = $2; }
//	|	LBRACE SEMICOLON case_list RBRACE				{ $$ = $3; }
//	|	COLON case_list ENDSWITCH SEMICOLON		{ $$ = $2; }
//	|	COLON SEMICOLON case_list ENDSWITCH SEMICOLON	{ $$ = $3; }
//;

switch_case_list(A) ::= LBRACE case_list(B) RBRACE. { A = B; }
switch_case_list(A) ::=	LBRACE SEMICOLON case_list(B) RBRACE. { A = B; }
switch_case_list(A) ::=	COLON case_list(B) ENDSWITCH SEMICOLON. { A = B; }
switch_case_list(A) ::=	COLON SEMICOLON case_list(B) ENDSWITCH SEMICOLON. { A = B; }

//case_list:
//		/* empty */	{ $$.op_type = IS_UNUSED; }
//	|	case_list CASE expr case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&$1, &$2, &$3 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
//	|	case_list DEFAULT case_separator { zend_do_extended_info(TSRMLS_C);  zend_do_default_before_statement(&$1, &$2 TSRMLS_CC); } inner_statement_list { zend_do_case_after_statement(&$$, &$2 TSRMLS_CC); $$.op_type = IS_CONST; }
//;

case_list_i(A)  ::= case_list(B) CASE(C) expr(D) case_separator. { zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&B, &C, &D TSRMLS_CC); A = C;}
case_list_ii(A) ::= case_list(B) DEFAULT(C) case_separator. { zend_do_extended_info(TSRMLS_C);  zend_do_default_before_statement(&B, &C TSRMLS_CC); A = C; }

case_list(A) ::= . { A.op_type = IS_UNUSED; }
case_list(A) ::= case_list_i(B) inner_statement_list. { zend_do_case_after_statement(&A, &B TSRMLS_CC); A.op_type = IS_CONST; }
case_list(A) ::= case_list_ii(B) inner_statement_list. { zend_do_case_after_statement(&A, &B TSRMLS_CC); A.op_type = IS_CONST; }

//case_separator:
//		COLON
//	|	SEMICOLON
//;

case_separator ::= COLON.
case_separator ::= SEMICOLON.

//while_statement:
//		statement
//	|	COLON inner_statement_list ENDWHILE SEMICOLON
//;

while_statement ::= statement.
while_statement ::= COLON inner_statement_list ENDWHILE SEMICOLON.

//elseif_list:
//		/* empty */
//	|	elseif_list ELSEIF LPAREN expr RPAREN { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } statement { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
//;

elseif_list_i(A) ::= elseif_list ELSEIF LPAREN expr(B) RPAREN(C). { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }

elseif_list ::= .
elseif_list ::= elseif_list_i(B) statement. { zend_do_if_after_statement(&B, 0 TSRMLS_CC); }

//elseif_alt_list:
//		/* empty */
//	|	elseif_alt_list ELSEIF LPAREN expr RPAREN COLON { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
//;

elseif_alt_list_i(A) ::= elseif_alt_list ELSEIF LPAREN expr(B) RPAREN(C) COLON. { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }

elseif_alt_list ::= .
elseif_alt_list ::= elseif_alt_list_i(B) inner_statement_list. { zend_do_if_after_statement(&B, 0 TSRMLS_CC); }

//else_single:
//		/* empty */
//	|	ELSE statement
//;

else_single ::= .
else_single ::= ELSE statement.

//else_alt_single:
//		/* empty */
//	|	ELSE COLON inner_statement_list
//;

else_alt_single ::= .
else_alt_single ::= ELSE COLON inner_statement_list.

//parameter_list:
//		non_empty_parameter_list
//	|	/* empty */
//;

parameter_list ::= .
parameter_list ::= non_empty_parameter_list.

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

non_empty_parameter_list(A) ::= optional_class_type(B) VARIABLE(C). { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &C, &A, NULL, &B, 0 TSRMLS_CC); }
non_empty_parameter_list(A) ::= optional_class_type(B) BW_AND VARIABLE(C). { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &C, &A, NULL, &B, 1 TSRMLS_CC); }
non_empty_parameter_list(A) ::= optional_class_type(B) BW_AND VARIABLE(C) EQUAL static_scalar(D). { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &C, &A, &D, &B, 1 TSRMLS_CC); }
non_empty_parameter_list(A) ::= optional_class_type(B) VARIABLE(C) EQUAL static_scalar(D). { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &C, &A, &D, &B, 0 TSRMLS_CC); }
non_empty_parameter_list(A) ::= non_empty_parameter_list(B) COMMA optional_class_type(C) VARIABLE(D). { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV, &D, &A, NULL, &C, 0 TSRMLS_CC); }
non_empty_parameter_list(A) ::= non_empty_parameter_list(B) COMMA optional_class_type(C) BW_AND VARIABLE(D). { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV, &D, &A, NULL, &C, 1 TSRMLS_CC); }
non_empty_parameter_list(A) ::= non_empty_parameter_list(B) COMMA optional_class_type(C) BW_AND VARIABLE(D) EQUAL static_scalar(E). { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &D, &A, &E, &C, 1 TSRMLS_CC); }
non_empty_parameter_list(A) ::= non_empty_parameter_list(B) COMMA optional_class_type(C) VARIABLE(D) EQUAL static_scalar(E). { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &D, &A, &E, &C, 0 TSRMLS_CC); }

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

optional_class_type(A) ::= . { A.op_type = IS_UNUSED; }
optional_class_type(A) ::= ARRAY(B).						{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_ARRAY; }
optional_class_type(A) ::= BOOL_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_BOOL; }
optional_class_type(A) ::= STRING_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_STRING; }
optional_class_type(A) ::= INT_HINT(B).						{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_LONG; }
optional_class_type(A) ::= DOUBLE_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_DOUBLE; }
optional_class_type(A) ::= RESOURCE_HINT(B).				{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_RESOURCE; }
optional_class_type(A) ::= OBJECT_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_OBJECT; }
optional_class_type(A) ::= SCALAR_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_SCALAR; }
optional_class_type(A) ::= NUMERIC_HINT(B).					{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_NUMERIC; }
optional_class_type(A) ::= fully_qualified_class_name(B).	{ A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_CLASS; }

//function_call_parameter_list:
//		non_empty_function_call_parameter_list	{ $$ = $1; }
//	|	/* empty */				{ Z_LVAL($$.u.constant) = 0; }
//;

function_call_parameter_list(A) ::= . { Z_LVAL(A.u.constant) = 0; }
function_call_parameter_list(A) ::= non_empty_function_call_parameter_list(B). { A = B; }

//non_empty_function_call_parameter_list:
//		expr_without_variable	{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	variable				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	'&' w_variable 				{ Z_LVAL($$.u.constant) = 1;  zend_do_pass_param(&$2, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA expr_without_variable	{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAL, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA variable					{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$3, ZEND_SEND_VAR, Z_LVAL($$.u.constant) TSRMLS_CC); }
//	|	non_empty_function_call_parameter_list COMMA '&' w_variable			{ Z_LVAL($$.u.constant)=Z_LVAL($1.u.constant)+1;  zend_do_pass_param(&$4, ZEND_SEND_REF, Z_LVAL($$.u.constant) TSRMLS_CC); }
//;

non_empty_function_call_parameter_list(A) ::= expr_without_variable(B).	{ Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_VAL, Z_LVAL(A.u.constant) TSRMLS_CC); }
non_empty_function_call_parameter_list(A) ::= variable(B).	{ Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_VAR, Z_LVAL(A.u.constant) TSRMLS_CC); }
non_empty_function_call_parameter_list(A) ::= BW_AND w_variable(B).	{ Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_REF, Z_LVAL(A.u.constant) TSRMLS_CC); }
non_empty_function_call_parameter_list(A) ::= non_empty_function_call_parameter_list(B) COMMA expr_without_variable(C).	{ Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_VAL, Z_LVAL(A.u.constant) TSRMLS_CC); }
non_empty_function_call_parameter_list(A) ::= non_empty_function_call_parameter_list(B) COMMA variable(C).	{ Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_VAR, Z_LVAL(A.u.constant) TSRMLS_CC); }
non_empty_function_call_parameter_list(A) ::= non_empty_function_call_parameter_list(B) COMMA BW_AND w_variable(C).	{ Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_REF, Z_LVAL(A.u.constant) TSRMLS_CC); }

//global_var_list:
//		global_var_list COMMA global_var	{ zend_do_fetch_global_variable(&$3, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
//	|	global_var						{ zend_do_fetch_global_variable(&$1, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
//;

global_var_list ::= global_var_list COMMA global_var(B). { zend_do_fetch_global_variable(&B, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
global_var_list ::= global_var(B). { zend_do_fetch_global_variable(&B, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }

//global_var:
//		VARIABLE			{ $$ = $1; }
//	|	'$' r_variable		{ $$ = $2; }
//	|	'$' LBRACE expr RBRACE	{ $$ = $3; }
//;

global_var(A) ::= VARIABLE(B).	{ A = B; }
global_var(A) ::= DOLLAR r_variable(B).	{ A = B; }
global_var(A) ::= DOLLAR LBRACE expr(B) RBRACE.	{ A = B; }

//static_var_list:
//		static_var_list COMMA VARIABLE { zend_do_fetch_static_variable(&$3, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	static_var_list COMMA VARIABLE EQUAL static_scalar { zend_do_fetch_static_variable(&$3, &$5, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	VARIABLE  { zend_do_fetch_static_variable(&$1, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
//	|	VARIABLE EQUAL static_scalar { zend_do_fetch_static_variable(&$1, &$3, ZEND_FETCH_STATIC TSRMLS_CC); }
//;

static_var_list ::= static_var_list COMMA VARIABLE(B). { zend_do_fetch_static_variable(&B, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= static_var_list COMMA VARIABLE(B) EQUAL static_scalar(C). { zend_do_fetch_static_variable(&B, &C, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= VARIABLE(B).  { zend_do_fetch_static_variable(&B, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= VARIABLE(B) EQUAL static_scalar(C). { zend_do_fetch_static_variable(&B, &C, ZEND_FETCH_STATIC TSRMLS_CC); }

//class_statement_list:
//		class_statement_list class_statement
//	|	/* empty */
//;

class_statement_list ::= .
class_statement_list ::= class_statement_list class_statement.

//class_statement:
//		variable_modifiers { CG(access_type) = Z_LVAL($1.u.constant); } class_variable_declaration SEMICOLON
//	|	class_constant_declaration SEMICOLON
//	|	trait_use_statement
//	|	method_modifiers function is_reference STRING { zend_do_begin_function_declaration(&$2, &$4, 1, $3.op_type, &$1 TSRMLS_CC); } LPAREN
//			parameter_list RPAREN method_body { zend_do_abstract_method(&$4, &$1, &$9 TSRMLS_CC); zend_do_end_function_declaration(&$2 TSRMLS_CC); }
//;

class_statement_i ::= variable_modifiers(B). { CG(access_type) = Z_LVAL(B.u.constant); }

class_statement_ii(A) ::= method_modifiers(B) function(C) is_reference(D) STRING(E). { zend_do_begin_function_declaration(&C, &E, 1, D.op_type, &B TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }

class_statement ::= class_statement_i class_variable_declaration SEMICOLON.
class_statement ::= class_constant_declaration SEMICOLON.
class_statement ::= trait_use_statement.
class_statement ::= class_statement_ii(B) LPAREN parameter_list RPAREN method_body(C). { zend_do_abstract_method(&B[2], &B[0], &C TSRMLS_CC); zend_do_end_function_declaration(&B[1] TSRMLS_CC); }

//trait_use_statement:
//		USE trait_list trait_adaptations
//;

trait_use_statement ::= USE trait_list trait_adaptations.

//trait_list:
//		fully_qualified_class_name						{ zend_do_implements_trait(&$1 TSRMLS_CC); }
//	|	trait_list COMMA fully_qualified_class_name		{ zend_do_implements_trait(&$3 TSRMLS_CC); }
//;

trait_list ::= fully_qualified_class_name(B).					{ zend_do_implements_trait(&B TSRMLS_CC); }
trait_list ::= trait_list COMMA fully_qualified_class_name(B).	{ zend_do_implements_trait(&B TSRMLS_CC); }

//trait_adaptations:
//		SEMICOLON
//	|	LBRACE trait_adaptation_list RBRACE
//;

trait_adaptations ::= SEMICOLON.
trait_adaptations ::= LBRACE trait_adaptation_list RBRACE.

//trait_adaptation_list:
//		/* empty */
//	|	non_empty_trait_adaptation_list
//;

trait_adaptation_list ::= .
trait_adaptation_list ::= non_empty_trait_adaptation_list.

//non_empty_trait_adaptation_list:
//		trait_adaptation_statement
//	|	non_empty_trait_adaptation_list trait_adaptation_statement
//;

non_empty_trait_adaptation_list ::= trait_adaptation_statement.
non_empty_trait_adaptation_list ::= non_empty_trait_adaptation_list trait_adaptation_statement.

//trait_adaptation_statement:
//		trait_precedence SEMICOLON								{ zend_add_trait_precedence(&$1 TSRMLS_CC); }
//	|	trait_alias SEMICOLON										{ zend_add_trait_alias(&$1 TSRMLS_CC); }
//;

trait_adaptation_statement ::= trait_precedence(B) SEMICOLON. { zend_add_trait_precedence(&B TSRMLS_CC); }
trait_adaptation_statement ::= trait_alias(B) SEMICOLON.  	{ zend_add_trait_alias(&B TSRMLS_CC); }

//trait_precedence:
//	trait_method_reference_fully_qualified INSTEADOF trait_reference_list	{ zend_prepare_trait_precedence(&$$, &$1, &$3 TSRMLS_CC); }
//;

trait_precedence(A) ::= trait_method_reference_fully_qualified(B) INSTEADOF trait_reference_list(C). { zend_prepare_trait_precedence(&A, &B, &C TSRMLS_CC); }

//trait_reference_list:
//		fully_qualified_class_name									{ zend_init_list(&$$.u.op.ptr, Z_STRVAL($1.u.constant) TSRMLS_CC); }
//	|	trait_reference_list COMMA fully_qualified_class_name			{ zend_add_to_list(&$1.u.op.ptr, Z_STRVAL($3.u.constant) TSRMLS_CC); $$ = $1; }
//;

trait_reference_list(A) ::= fully_qualified_class_name(B). { zend_init_list(&A.u.op.ptr, Z_STRVAL(B.u.constant) TSRMLS_CC); }
trait_reference_list(A) ::= trait_reference_list(B) COMMA fully_qualified_class_name(C). { zend_add_to_list(&B.u.op.ptr, Z_STRVAL(C.u.constant) TSRMLS_CC); A = B; }

//trait_method_reference:
//		STRING													{ zend_prepare_reference(&$$, NULL, &$1 TSRMLS_CC); }
//	|	trait_method_reference_fully_qualified						{ $$ = $1; }
//;

trait_method_reference(A) ::= STRING(B). { zend_prepare_reference(&A, NULL, &B TSRMLS_CC); }
trait_method_reference(A) ::= trait_method_reference_fully_qualified(B). { A = B; }

//trait_method_reference_fully_qualified:
//	fully_qualified_class_name PAAMAYIM_NEKUDOTAYIM STRING		{ zend_prepare_reference(&$$, &$1, &$3 TSRMLS_CC); }
//;

trait_method_reference_fully_qualified(A) ::= fully_qualified_class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C).	{ zend_prepare_reference(&A, &B, &C TSRMLS_CC); }

//trait_alias:
//		trait_method_reference AS trait_modifiers STRING		{ zend_prepare_trait_alias(&$$, &$1, &$3, &$4 TSRMLS_CC); }
//	|	trait_method_reference AS member_modifier					{ zend_prepare_trait_alias(&$$, &$1, &$3, NULL TSRMLS_CC); }
//;

trait_alias(A) ::= trait_method_reference(B) AS trait_modifiers(C) STRING(D). { zend_prepare_trait_alias(&A, &B, &C, &D TSRMLS_CC); }
trait_alias(A) ::= trait_method_reference(B) AS member_modifier(C). { zend_prepare_trait_alias(&A, &B, &C, NULL TSRMLS_CC); }

//trait_modifiers:
//		/* empty */					{ Z_LVAL($$.u.constant) = 0x0; } /* No change of methods visibility */
//	|	member_modifier	{ $$ = $1; } /* REM: Keep in mind, there are not only visibility modifiers */
//;

trait_modifiers(A) ::= . { Z_LVAL(A.u.constant) = 0x0; } /* No change of methods visibility */
trait_modifiers(A) ::= member_modifier(B).	{ A = B; } /* REM: Keep in mind, there are not only visibility modifiers */

//method_body:
//		SEMICOLON /* abstract method */		{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
//	|	LBRACE inner_statement_list RBRACE	{ Z_LVAL($$.u.constant) = 0;	}
//;

method_body(A) ::= SEMICOLON. { Z_LVAL(A.u.constant) = ZEND_ACC_ABSTRACT; }
method_body(A) ::= LBRACE inner_statement_list RBRACE.	{ Z_LVAL(A.u.constant) = 0;	}

//variable_modifiers:
//		non_empty_member_modifiers		{ $$ = $1; }
//	|	VAR							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//;

variable_modifiers(A) ::= non_empty_member_modifiers(B). { A = B; }
variable_modifiers(A) ::= VAR.	{ Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }

//method_modifiers:
//		/* empty */							{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//	|	non_empty_member_modifiers			{ $$ = $1;  if (!(Z_LVAL($$.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL($$.u.constant) |= ZEND_ACC_PUBLIC; } }
//;

method_modifiers(A) ::= . { Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }
method_modifiers(A) ::= non_empty_member_modifiers(B). { A = B;  if (!(Z_LVAL(A.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL(A.u.constant) |= ZEND_ACC_PUBLIC; } }

//non_empty_member_modifiers:
//		member_modifier						{ $$ = $1; }
//	|	non_empty_member_modifiers member_modifier	{ Z_LVAL($$.u.constant) = zend_do_verify_access_types(&$1, &$2); }
//;

non_empty_member_modifiers(A) ::= member_modifier(B). { A = B; }
non_empty_member_modifiers(A) ::= non_empty_member_modifiers(B) member_modifier(C). { Z_LVAL(A.u.constant) = zend_do_verify_access_types(&B, &C); }

//member_modifier:
//		PUBLIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_PUBLIC; }
//	|	PROTECTED				{ Z_LVAL($$.u.constant) = ZEND_ACC_PROTECTED; }
//	|	PRIVATE				{ Z_LVAL($$.u.constant) = ZEND_ACC_PRIVATE; }
//	|	STATIC				{ Z_LVAL($$.u.constant) = ZEND_ACC_STATIC; }
//	|	ABSTRACT				{ Z_LVAL($$.u.constant) = ZEND_ACC_ABSTRACT; }
//	|	FINAL					{ Z_LVAL($$.u.constant) = ZEND_ACC_FINAL; }
//;

member_modifier(A) ::= PUBLIC.		{ Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }
member_modifier(A) ::= PROTECTED. 	{ Z_LVAL(A.u.constant) = ZEND_ACC_PROTECTED; }
member_modifier(A) ::= PRIVATE. 	{ Z_LVAL(A.u.constant) = ZEND_ACC_PRIVATE; }
member_modifier(A) ::= STATIC.		{ Z_LVAL(A.u.constant) = ZEND_ACC_STATIC; }
member_modifier(A) ::= ABSTRACT. 	{ Z_LVAL(A.u.constant) = ZEND_ACC_ABSTRACT; }
member_modifier(A) ::= FINAL.		{ Z_LVAL(A.u.constant) = ZEND_ACC_FINAL; }

//class_variable_declaration:
//		class_variable_declaration COMMA VARIABLE					{ zend_do_declare_property(&$3, NULL, CG(access_type) TSRMLS_CC); }
//	|	class_variable_declaration COMMA VARIABLE EQUAL static_scalar	{ zend_do_declare_property(&$3, &$5, CG(access_type) TSRMLS_CC); }
//	|	VARIABLE						{ zend_do_declare_property(&$1, NULL, CG(access_type) TSRMLS_CC); }
//	|	VARIABLE EQUAL static_scalar	{ zend_do_declare_property(&$1, &$3, CG(access_type) TSRMLS_CC); }
//;

class_variable_declaration ::= class_variable_declaration COMMA VARIABLE(B). { zend_do_declare_property(&B, NULL, CG(access_type) TSRMLS_CC); }
class_variable_declaration ::= class_variable_declaration COMMA VARIABLE(B) EQUAL static_scalar(C). { zend_do_declare_property(&B, &C, CG(access_type) TSRMLS_CC); }
class_variable_declaration ::= VARIABLE(B). { zend_do_declare_property(&B, NULL, CG(access_type) TSRMLS_CC); }
class_variable_declaration ::= VARIABLE(B) EQUAL static_scalar(C).	{ zend_do_declare_property(&B, &C, CG(access_type) TSRMLS_CC); }

//class_constant_declaration:
//		class_constant_declaration COMMA STRING EQUAL static_scalar	{ zend_do_declare_class_constant(&$3, &$5 TSRMLS_CC); }
//	|	CONST STRING EQUAL static_scalar	{ zend_do_declare_class_constant(&$2, &$4 TSRMLS_CC); }
//;

class_constant_declaration ::= class_constant_declaration COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_class_constant(&B, &C TSRMLS_CC); }
class_constant_declaration ::= CONST STRING(B) EQUAL static_scalar(C). { zend_do_declare_class_constant(&B, &C TSRMLS_CC); }

//echo_expr_list:
//		echo_expr_list COMMA expr { zend_do_echo(&$3 TSRMLS_CC); }
//	|	expr					{ zend_do_echo(&$1 TSRMLS_CC); }
//;

echo_expr_list ::= expr(A). { zend_do_echo(&A TSRMLS_CC); }
echo_expr_list ::= echo_expr_list COMMA expr(A). { zend_do_echo(&A TSRMLS_CC); }

//for_expr:
//		/* empty */			{ $$.op_type = IS_CONST;  Z_TYPE($$.u.constant) = IS_BOOL;  Z_LVAL($$.u.constant) = 1; }
//	|	non_empty_for_expr	{ $$ = $1; }
//;

for_expr(A) ::= . { A.op_type = IS_CONST;  Z_TYPE(A.u.constant) = IS_BOOL;  Z_LVAL(A.u.constant) = 1; }
for_expr(A) ::= non_empty_for_expr(B). { A = B; }

//non_empty_for_expr:
//		non_empty_for_expr COMMA	{ zend_do_free(&$1 TSRMLS_CC); } expr { $$ = $4; }
//	|	expr					{ $$ = $1; }
//;

non_empty_for_expr_i ::= non_empty_for_expr(B) COMMA. { zend_do_free(&B TSRMLS_CC); }

non_empty_for_expr(A) ::= non_empty_for_expr_i expr(B). { A = B; }
non_empty_for_expr(A) ::= expr(B). { A = B; }

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

new_i(A) ::= NEW(B) class_name_reference(C). { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&B, &C TSRMLS_CC); A = B; } 
new_ii(A) ::= variable(B) EQUAL BW_AND(C) NEW(D) class_name_reference(E). {
	zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");
	zend_check_writable_variable(&B);
	zend_do_extended_fcall_begin(TSRMLS_C);
	zend_do_begin_new_object(&D, &E TSRMLS_CC);
	A[0] = B;
	A[1] = C;
	A[2] = D;
}

list_i ::= LIST LPAREN. { zend_do_list_init(TSRMLS_C); }
list(A) ::= list_i assignment_list RPAREN EQUAL expr(B). { zend_do_list_end(&A, &B TSRMLS_CC); }

expr_without_variable(A) ::= list(B). { A = B; }
expr_without_variable(A) ::= variable(B) EQUAL expr(C).		{ zend_check_writable_variable(&B); zend_do_assign(&A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) EQUAL BW_AND variable(C). { zend_check_writable_variable(&B); zend_do_end_variable_parse(&C, BP_VAR_W, 1 TSRMLS_CC); zend_do_end_variable_parse(&B, BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= new_ii(B) ctor_arguments(C). { zend_do_end_new_object(&B[1], &B[2], &C TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(&B[0], BP_VAR_W, 0 TSRMLS_CC); B[1].EA = ZEND_PARSED_NEW; zend_do_assign_ref(&A, &B[0], &B[1] TSRMLS_CC); }
expr_without_variable(A) ::= new_i(B) ctor_arguments(C). { zend_do_end_new_object(&A, &B, &C TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
expr_without_variable(A) ::= CLONE expr(B). { zend_do_clone(&A, &B TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) PLUS_EQUAL expr(C). 	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_ADD, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) MINUS_EQUAL expr(C).	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SUB, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) MUL_EQUAL expr(C).		{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MUL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) DIV_EQUAL expr(C).		{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_DIV, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) CONCAT_EQUAL expr(C).	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_CONCAT, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) MOD_EQUAL expr(C).		{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MOD, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) AND_EQUAL expr(C).		{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_AND, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) OR_EQUAL expr(C). 		{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_OR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) XOR_EQUAL expr(C). 	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) SL_EQUAL expr(C).	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= variable(B) SR_EQUAL expr(C).	{ zend_check_writable_variable(&B); zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= rw_variable(B) INC. { zend_do_post_incdec(&A, &B, ZEND_POST_INC TSRMLS_CC); }
expr_without_variable(A) ::= INC rw_variable(B). { zend_do_pre_incdec(&A, &B, ZEND_PRE_INC TSRMLS_CC); }
expr_without_variable(A) ::= rw_variable(B) DEC. { zend_do_post_incdec(&A, &B, ZEND_POST_DEC TSRMLS_CC); }
expr_without_variable(A) ::= DEC rw_variable(B). { zend_do_pre_incdec(&A, &B, ZEND_PRE_DEC TSRMLS_CC); }
expr_without_variable(A) ::= bool_or_expr(B). { A = B; }
expr_without_variable(A) ::= bool_and_expr(B). { A = B; }
expr_without_variable(A) ::= logic_or_expr(B). { A = B; }
expr_without_variable(A) ::= logic_and_expr(B). { A = B; }
expr_without_variable(A) ::= expr(B) LOGICAL_XOR expr(C). { zend_do_binary_op(ZEND_BOOL_XOR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) BW_OR expr(C).	{ zend_do_binary_op(ZEND_BW_OR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) BW_AND expr(C).	{ zend_do_binary_op(ZEND_BW_AND, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) BW_XOR expr(C).	{ zend_do_binary_op(ZEND_BW_XOR, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) DOT expr(C). 	{ zend_do_binary_op(ZEND_CONCAT, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) PLUS expr(C). 	{ zend_do_binary_op(ZEND_ADD, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) MINUS expr(C). 	{ zend_do_binary_op(ZEND_SUB, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) MULT expr(C).	{ zend_do_binary_op(ZEND_MUL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) DIV expr(C).	{ zend_do_binary_op(ZEND_DIV, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) MOD expr(C).	{ zend_do_binary_op(ZEND_MOD, &A, &B, &C TSRMLS_CC); }
//expr_without_variable(A) ::= expr(B) SL expr(C). 	{ zend_do_binary_op(ZEND_SL, &A, &B, &C TSRMLS_CC); }
//expr_without_variable(A) ::= expr(B) SR expr(C).	{ zend_do_binary_op(ZEND_SR, &A, &B, &C TSRMLS_CC); }
//expr_without_variable(A) ::= PLUS(B) expr(C). [INC] { ZVAL_LONG(&B.u.constant, 0); if (C.op_type == IS_CONST) { add_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); A = C; } else { B.op_type = IS_CONST; INIT_PZVAL(&B.u.constant); zend_do_binary_op(ZEND_ADD, &A, &B, &C TSRMLS_CC); } }
//expr_without_variable(A) ::= MINUS(B) expr(C). [INC] { ZVAL_LONG(&B.u.constant, 0); if (C.op_type == IS_CONST) { sub_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); A = C; } else { B.op_type = IS_CONST; INIT_PZVAL(&B.u.constant); zend_do_binary_op(ZEND_SUB, &A, &B, &C TSRMLS_CC); } }
expr_without_variable(A) ::= BOOL_NOT expr(B). { zend_do_unary_op(ZEND_BOOL_NOT, &A, &B TSRMLS_CC); }
expr_without_variable(A) ::= BW_NOT expr(B). { zend_do_unary_op(ZEND_BW_NOT, &A, &B TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_IDENTICAL expr(C).	{ zend_do_binary_op(ZEND_IS_IDENTICAL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_NOT_IDENTICAL expr(C). { zend_do_binary_op(ZEND_IS_NOT_IDENTICAL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_EQUAL expr(C).	{ zend_do_binary_op(ZEND_IS_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_NOT_EQUAL expr(C).	{ zend_do_binary_op(ZEND_IS_NOT_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_SMALLER expr(C).  { zend_do_binary_op(ZEND_IS_SMALLER, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_SMALLER_OR_EQUAL expr(C). { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_GREATER expr(C). { zend_do_binary_op(ZEND_IS_SMALLER, &A, &C, &B TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) IS_GREATER_OR_EQUAL expr(C). { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &A, &C, &B TSRMLS_CC); }
expr_without_variable(A) ::= expr(B) INSTANCEOF class_name_reference(C). { zend_do_instanceof(&A, &B, &C, 0 TSRMLS_CC); }
expr_without_variable(A) ::= LPAREN expr(B) RPAREN.	{ A = B; }
expr_without_variable(A) ::= ternary(B). { A = B; }
expr_without_variable(A) ::= short_ternary(B). { A = B; }
expr_without_variable(A) ::= internal_functions_in_yacc(B). { A = B; }
expr_without_variable(A) ::= INT_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_LONG TSRMLS_CC); }
expr_without_variable(A) ::= DOUBLE_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_DOUBLE TSRMLS_CC); }
expr_without_variable(A) ::= STRING_CAST expr(B).	{ zend_do_cast(&A, &B, IS_STRING TSRMLS_CC); }
expr_without_variable(A) ::= ARRAY_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_ARRAY TSRMLS_CC); }
expr_without_variable(A) ::= OBJECT_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_OBJECT TSRMLS_CC); }
expr_without_variable(A) ::= BOOL_CAST expr(B).	{ zend_do_cast(&A, &B, IS_BOOL TSRMLS_CC); }
expr_without_variable(A) ::= SCALAR_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_SCALAR TSRMLS_CC); }
expr_without_variable(A) ::= NUMERIC_CAST expr(B). 	{ zend_do_cast(&A, &B, IS_NUMERIC TSRMLS_CC); }
expr_without_variable(A) ::= UNSET_CAST expr(B).	{ zend_do_cast(&A, &B, IS_NULL TSRMLS_CC); }
expr_without_variable(A) ::= EXIT exit_expr(B).	{ zend_do_exit(&A, &B TSRMLS_CC); }
expr_without_variable(A) ::= silent(B) expr(C). [IF] { zend_do_end_silence(&B TSRMLS_CC); A = C; }
expr_without_variable(A) ::= ARRAY LPAREN array_pair_list(B) RPAREN. { A = B; }
expr_without_variable(A) ::= scalar(B). { A = B; }
expr_without_variable(A) ::= BACKQUOTE backticks_expr(B) BACKQUOTE. { zend_do_shell_exec(&A, &B TSRMLS_CC); }
expr_without_variable(A) ::= PRINT expr(B). { zend_do_print(&A, &B TSRMLS_CC); }
expr_without_variable(A) ::= closure_i(B) parameter_list RPAREN lexical_vars LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B[1] TSRMLS_CC); A = B[0]; }
expr_without_variable(A) ::= closure_ii(B) parameter_list RPAREN lexical_vars LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B[1] TSRMLS_CC); A = B[0]; }

bool_or_expr_i(A) ::= expr(B) BOOLEAN_OR(C). { zend_do_boolean_or_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }
bool_or_expr(A) ::= bool_or_expr_i(B) expr(C). { zend_do_boolean_or_end(&A, &B[0], &C, &B[1] TSRMLS_CC); }

bool_and_expr_i(A) ::= expr(B) BOOLEAN_AND(C). { zend_do_boolean_and_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }
bool_and_expr(A) ::=  bool_and_expr_i(B) expr(C). { zend_do_boolean_and_end(&A, &B[0], &C, &B[1] TSRMLS_CC); }

logic_or_expr_i(A) ::= expr(B) LOGICAL_OR(C). { zend_do_boolean_or_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }
logic_or_expr(A) ::= logic_or_expr_i(B) expr(C). { zend_do_boolean_or_end(&A, &B[0], &C, &B[1] TSRMLS_CC); }

logic_and_expr_i(A) ::= expr(B) LOGICAL_AND(C). { zend_do_boolean_and_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }
logic_and_expr(A) ::= logic_and_expr_i(B) expr(C). { zend_do_boolean_and_end(&A, &B[0], &C, &B[1] TSRMLS_CC); }

ternary_ii(A) ::= expr(B) QUESTION_MARK(C). { zend_do_begin_qm_op(&B, &C TSRMLS_CC); A = C; }
ternary_i(A)  ::= ternary_ii(B) expr(C) COLON(D). { zend_do_qm_true(&C, &B, &D TSRMLS_CC); A[0] = B; A[1] = D; }
ternary(A)    ::= ternary_i(B) expr(C).	 { zend_do_qm_false(&A, &C, &B[0], &B[1] TSRMLS_CC); }

short_ternary_i(A) ::= expr(B) QUESTION_MARK(C) COLON(D). { zend_do_jmp_set(&B, &C, &D TSRMLS_CC); A[0] = C; A[1] = D; }
short_ternary(A)   ::= short_ternary_i(B) expr(C). { zend_do_jmp_set_else(&A, &C, &B[0], &B[1] TSRMLS_CC); }

closure_i(A) ::= function(B) is_reference(C) LPAREN. { zend_do_begin_lambda_function_declaration(&A[0], &B, C.op_type, 0 TSRMLS_CC); A[1] = B; }
closure_ii(A) ::= STATIC function(B) is_reference(C) LPAREN. { zend_do_begin_lambda_function_declaration(&A[0], &B, C.op_type, 1 TSRMLS_CC); A[1] = B; }

silent(A) ::= AT(B). { zend_do_begin_silence(&B TSRMLS_CC); A = B; }

//function:
//	FUNCTION { $$.u.op.opline_num = CG(zend_lineno); }
//;

function(A) ::= FUNCTION. { A.u.op.opline_num = CG(zend_lineno); }

//lexical_vars:
//		/* empty */
//	|	USE LPAREN lexical_var_list RPAREN
//;

lexical_vars ::= .
lexical_vars ::= USE LPAREN lexical_var_list RPAREN.

//lexical_var_list:
//		lexical_var_list COMMA VARIABLE			{ zend_do_fetch_lexical_variable(&$3, 0 TSRMLS_CC); }
//	|	lexical_var_list COMMA '&' VARIABLE		{ zend_do_fetch_lexical_variable(&$4, 1 TSRMLS_CC); }
//	|	VARIABLE								{ zend_do_fetch_lexical_variable(&$1, 0 TSRMLS_CC); }
//	|	'&' VARIABLE							{ zend_do_fetch_lexical_variable(&$2, 1 TSRMLS_CC); }
//;

lexical_var_list ::= lexical_var_list COMMA VARIABLE(B).	{ zend_do_fetch_lexical_variable(&B, 0 TSRMLS_CC); }
lexical_var_list ::= lexical_var_list COMMA BW_AND VARIABLE(B).	{ zend_do_fetch_lexical_variable(&B, 1 TSRMLS_CC); }
lexical_var_list ::= VARIABLE(B).								{ zend_do_fetch_lexical_variable(&B, 0 TSRMLS_CC); }
lexical_var_list ::= BW_AND VARIABLE(B).						{ zend_do_fetch_lexical_variable(&B, 1 TSRMLS_CC); }

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

function_call_ns_i(A) ::= namespace_name(B) LPAREN(C). { C.u.op.opline_num = zend_do_begin_function_call(&B, 1 TSRMLS_CC); A[0] = B; A[1] = C; }

function_call_ns_ii(A) ::= NAMESPACE(B) NS_SEPARATOR namespace_name(C) LPAREN(D). {
	B.op_type = IS_CONST;
	ZVAL_EMPTY_STRING(&B.u.constant);
	zend_do_build_namespace_name(&B, &B, &C TSRMLS_CC);
	D.u.op.opline_num = zend_do_begin_function_call(&B, 0 TSRMLS_CC);
	A[0] = B;
	A[1] = D;
}

function_call_ns_iii(A) ::= NS_SEPARATOR namespace_name(B) LPAREN(C). { C.u.op.opline_num = zend_do_begin_function_call(&B, 0 TSRMLS_CC); A[0] = B; A[1] = C; }

function_call_cls_i(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C) LPAREN(D). { D.u.op.opline_num = zend_do_begin_class_member_function_call(&B, &C TSRMLS_CC); A[0] = C; A[1] = D; }

function_call_cls_ii ::= class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C) LPAREN. { zend_do_end_variable_parse(&C, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_class_member_function_call(&B, &C TSRMLS_CC); }

function_call_var_i ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C) LPAREN. { zend_do_begin_class_member_function_call(&B, &C TSRMLS_CC); }

function_call_var_ii ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C) LPAREN. {
	zend_do_end_variable_parse(&C, BP_VAR_R, 0 TSRMLS_CC);
	zend_do_begin_class_member_function_call(&B, &C TSRMLS_CC);
}

function_call_var_iii(A) ::= variable_without_objects(B)  LPAREN. { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_dynamic_function_call(&B, 0 TSRMLS_CC); A = B; }

function_call(A) ::= function_call_ns_i(B) function_call_parameter_list(C) RPAREN. {
	zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
	zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_ns_ii(B)	function_call_parameter_list(C) RPAREN. {
	zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
	zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_ns_iii(B) function_call_parameter_list(C) RPAREN. {
	zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
	zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_cls_i(B) function_call_parameter_list(C) RPAREN. {
	zend_do_end_function_call(B[1].u.op.opline_num?NULL:&B[0], &A, &C, B[1].u.op.opline_num, B[1].u.op.opline_num TSRMLS_CC);
	zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_cls_ii function_call_parameter_list(C) RPAREN. { zend_do_end_function_call(NULL, &A, &C, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_i function_call_parameter_list(B) RPAREN. { zend_do_end_function_call(NULL, &A, &B, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_ii function_call_parameter_list(B) RPAREN. { zend_do_end_function_call(NULL, &A, &B, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_iii(B) function_call_parameter_list(C) RPAREN. { zend_do_end_function_call(&B, &A, &C, 0, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

//class_name:
//		STATIC { $$.op_type = IS_CONST; ZVAL_STRINGL(&$$.u.constant, "static", sizeof("static")-1, 1);}
//	|	namespace_name { $$ = $1; }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
//;

class_name(A) ::= STATIC. { A.op_type = IS_CONST; ZVAL_STRINGL(&A.u.constant, "static", sizeof("static")-1, 1); }
class_name(A) ::= namespace_name(B). { A = B; }
class_name(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); }
class_name(A) ::= NS_SEPARATOR namespace_name(B). { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL(B.u.constant)); Z_STRVAL(B.u.constant) = tmp; ++Z_STRLEN(B.u.constant); A = B; }

//fully_qualified_class_name:
//		namespace_name { $$ = $1; }
//	|	NAMESPACE NS_SEPARATOR namespace_name { $$.op_type = IS_CONST; ZVAL_EMPTY_STRING(&$$.u.constant);  zend_do_build_namespace_name(&$$, &$$, &$3 TSRMLS_CC); }
//	|	NS_SEPARATOR namespace_name { char *tmp = estrndup(Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL($2.u.constant), Z_STRLEN($2.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL($2.u.constant)); Z_STRVAL($2.u.constant) = tmp; ++Z_STRLEN($2.u.constant); $$ = $2; }
//;

fully_qualified_class_name(A) ::= namespace_name(B). { A = B; }
fully_qualified_class_name(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); }
fully_qualified_class_name(A) ::= NS_SEPARATOR namespace_name(B). { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL(B.u.constant)); Z_STRVAL(B.u.constant) = tmp; ++Z_STRLEN(B.u.constant); A = B; }

//class_name_reference:
//		class_name						{ zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
//	|	dynamic_class_name_reference	{ zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_fetch_class(&$$, &$1 TSRMLS_CC); }
//;

class_name_reference(A) ::= class_name(B). { zend_do_fetch_class(&A, &B TSRMLS_CC); }
class_name_reference(A) ::= dynamic_class_name_reference(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); zend_do_fetch_class(&A, &B TSRMLS_CC); }

//dynamic_class_name_reference:
//		base_variable OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
//			object_property { zend_do_push_object(&$4 TSRMLS_CC); } dynamic_class_name_variable_properties
//			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = ZEND_PARSED_MEMBER; }
//	|	base_variable { $$ = $1; }
//;

dynamic_class_name_reference_i ::= base_variable(B) OBJECT_OPERATOR. { zend_do_push_object(&B TSRMLS_CC); }

dynamic_class_name_reference_ii ::= dynamic_class_name_reference_i object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

dynamic_class_name_reference(A) ::= dynamic_class_name_reference_ii dynamic_class_name_variable_properties. {
	zend_do_pop_object(&A TSRMLS_CC);
	A.EA = ZEND_PARSED_MEMBER;
}
dynamic_class_name_reference(A) ::= base_variable(B). { A = B; }

//dynamic_class_name_variable_properties:
//		dynamic_class_name_variable_properties dynamic_class_name_variable_property
//	|	/* empty */
//;

dynamic_class_name_variable_properties ::= .
dynamic_class_name_variable_properties ::= dynamic_class_name_variable_properties dynamic_class_name_variable_property.

//dynamic_class_name_variable_property:
//		OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); }
//;

dynamic_class_name_variable_property ::= OBJECT_OPERATOR object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

//exit_expr:
//		/* empty */	{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
//	|	LPAREN RPAREN		{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }
//	|	LPAREN expr RPAREN	{ $$ = $2; }
//;

exit_expr ::= .
exit_expr(A) ::= LPAREN RPAREN.		{ memset(&A, 0, sizeof(znode)); A.op_type = IS_UNUSED; }
exit_expr(A) ::= LPAREN expr(B) RPAREN.	{ A = B; }

//backticks_expr:
//		/* empty */	{ ZVAL_EMPTY_STRING(&$$.u.constant); INIT_PZVAL(&$$.u.constant); $$.op_type = IS_CONST; }
//	|	ENCAPSED_AND_WHITESPACE	{ $$ = $1; }
//	|	encaps_list	{ $$ = $1; }
//;

backticks_expr(A) ::= . { ZVAL_EMPTY_STRING(&A.u.constant); INIT_PZVAL(&A.u.constant); A.op_type = IS_CONST; }
backticks_expr(A) ::= ENCAPSED_AND_WHITESPACE(B). { A = B; }
backticks_expr(A) ::= encaps_list(B). { A = B; }

//ctor_arguments:
//		/* empty */	{ Z_LVAL($$.u.constant)=0; }
//	|	LPAREN function_call_parameter_list RPAREN	{ $$ = $2; }
//;

ctor_arguments(A) ::= . { Z_LVAL(A.u.constant) = 0; }
ctor_arguments(A) ::= LPAREN function_call_parameter_list(B) RPAREN. { A = B; }

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
common_scalar(A) ::= START_HEREDOC(B) ENCAPSED_AND_WHITESPACE(C) END_HEREDOC. { A = C; CG(heredoc) = Z_STRVAL(B.u.constant); CG(heredoc_len) = Z_STRLEN(B.u.constant); }
common_scalar(A) ::= START_HEREDOC(B) END_HEREDOC. { ZVAL_EMPTY_STRING(&A.u.constant); INIT_PZVAL(&A.u.constant); A.op_type = IS_CONST; CG(heredoc) = Z_STRVAL(B.u.constant); CG(heredoc_len) = Z_STRLEN(B.u.constant); }

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
static_scalar(A) ::= namespace_name(B). { zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 1 TSRMLS_CC); }
static_scalar(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); B = A; zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 0 TSRMLS_CC); }
static_scalar(A) ::= NS_SEPARATOR namespace_name(B). { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL(B.u.constant)); Z_STRVAL(B.u.constant) = tmp; ++Z_STRLEN(B.u.constant); zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 0 TSRMLS_CC); }
static_scalar(A) ::= PLUS(B) static_scalar(C). { ZVAL_LONG(&B.u.constant, 0); add_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); A = C; }
static_scalar(A) ::= MINUS(B) static_scalar(C). { ZVAL_LONG(&C.u.constant, 0); sub_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); A = C; }
static_scalar(A) ::= ARRAY LPAREN static_array_pair_list(B) RPAREN. { A = B; Z_TYPE(A.u.constant) = IS_CONSTANT_ARRAY; }
static_scalar(A) ::= static_class_constant(B). { A = B; }

//static_class_constant:
//		class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_CT, 0 TSRMLS_CC); }
//;

static_class_constant(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C). { zend_do_fetch_constant(&A, &B, &C, ZEND_CT, 0 TSRMLS_CC); }

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

scalar(A) ::= STRING_VARNAME(B).	{ A = B; }
scalar(A) ::= class_constant(B).	{ A = B; }
scalar(A) ::= namespace_name(B).	{ zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 1 TSRMLS_CC); }
scalar(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); B = A; zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 0 TSRMLS_CC); }
scalar(A) ::= NS_SEPARATOR namespace_name(B). { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1); tmp[0] = '\\'; efree(Z_STRVAL(B.u.constant)); Z_STRVAL(B.u.constant) = tmp; ++Z_STRLEN(B.u.constant); zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 0 TSRMLS_CC); }
scalar(A) ::= common_scalar(B). { A = B; }
scalar(A) ::= QUOTE encaps_list(B) QUOTE. { A = B; }
scalar(A) ::= START_HEREDOC(B) encaps_list(C) END_HEREDOC. { A = C; CG(heredoc) = Z_STRVAL(B.u.constant); CG(heredoc_len) = Z_STRLEN(B.u.constant); }

//static_array_pair_list:
//		/* empty */ { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); }
//	|	non_empty_static_array_pair_list possible_comma	{ $$ = $1; }
//;

static_array_pair_list(A) ::= . { A.op_type = IS_CONST; INIT_PZVAL(&A.u.constant); array_init(&A.u.constant); }
static_array_pair_list(A) ::= non_empty_static_array_pair_list(B) possible_comma. { A = B; }

//possible_comma:
//		/* empty */
//	|	COMMA
//;

possible_comma ::= .
possible_comma ::= COMMA.

//non_empty_static_array_pair_list:
//		non_empty_static_array_pair_list COMMA static_scalar DOUBLE_ARROW static_scalar	{ zend_do_add_static_array_element(&$$, &$3, &$5); }
//	|	non_empty_static_array_pair_list COMMA static_scalar { zend_do_add_static_array_element(&$$, NULL, &$3); }
//	|	static_scalar DOUBLE_ARROW static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, &$1, &$3); }
//	|	static_scalar { $$.op_type = IS_CONST; INIT_PZVAL(&$$.u.constant); array_init(&$$.u.constant); zend_do_add_static_array_element(&$$, NULL, &$1); }
//;

non_empty_static_array_pair_list(A) ::= non_empty_static_array_pair_list(B) COMMA static_scalar(C) DOUBLE_ARROW static_scalar(D). { zend_do_add_static_array_element(&B, &C, &D); A = B; }
non_empty_static_array_pair_list(A) ::= non_empty_static_array_pair_list(B) COMMA static_scalar(C). { zend_do_add_static_array_element(&B, NULL, &C); A = B; }
non_empty_static_array_pair_list(A) ::= static_scalar(B) DOUBLE_ARROW static_scalar(C). { A.op_type = IS_CONST;
                                                                                          INIT_PZVAL(&A.u.constant);
                                                                                          array_init(&A.u.constant);
                                                                                          zend_do_add_static_array_element(&A, &B, &C); }

non_empty_static_array_pair_list(A) ::= static_scalar(B). { A.op_type = IS_CONST;
                                                            INIT_PZVAL(&A.u.constant);
                                                            array_init(&A.u.constant);
                                                            zend_do_add_static_array_element(&A, NULL, &B); }

//expr:
//		r_variable					{ $$ = $1; }
//	|	expr_without_variable		{ $$ = $1; }
//;

expr(A) ::= r_variable(B).	{ A = B; }
expr(A) ::= expr_without_variable(B).	{ A = B; }

//r_variable:
//	variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$ = $1; }
//;

r_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); A = B; }

//w_variable:
//	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_W, 0 TSRMLS_CC); $$ = $1;
//				  zend_check_writable_variable(&$1); }
//;

w_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_W, 0 TSRMLS_CC); A = B; zend_check_writable_variable(&B); }

//rw_variable:
//	variable	{ zend_do_end_variable_parse(&$1, BP_VAR_RW, 0 TSRMLS_CC); $$ = $1;
//				  zend_check_writable_variable(&$1); }
//;

rw_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); A = B; zend_check_writable_variable(&B); }

//variable:
//		base_variable_with_function_calls OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
//			object_property { zend_do_push_object(&$4 TSRMLS_CC); } method_or_not variable_properties
//			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.EA = $1.EA | ($7.EA ? $7.EA : $6.EA); }
//	|	base_variable_with_function_calls { $$ = $1; }
//;

variable_i(A) ::= base_variable_with_function_calls(B) OBJECT_OPERATOR. { zend_do_push_object(&B TSRMLS_CC); A = B; }

variable_ii ::= object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

variable(A) ::= variable_i(B) variable_ii method_or_not(C) variable_properties(D). { zend_do_pop_object(&A TSRMLS_CC); A.EA = B.EA | (D.EA ? D.EA : C.EA); }
variable(A) ::= base_variable_with_function_calls(B). { A = B; }

//variable_properties:
//		variable_properties variable_property { $$.EA = $2.EA; }
//	|	/* empty */ { $$.EA = 0; }
//;

variable_properties(A) ::= . { A.EA = 0; }
variable_properties(A) ::= variable_properties variable_property(B). { A.EA = B.EA; }

//variable_property:
//		OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); } method_or_not { $$.EA = $4.EA; }
//;

variable_property_i ::= OBJECT_OPERATOR object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

variable_property(A) ::= variable_property_i method_or_not(B). { A.EA = B.EA; }

//array_method_dereference:
//		array_method_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	method '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//;

array_method_dereference(A) ::= array_method_dereference(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
array_method_dereference(A) ::= method(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }

//method:
//		LPAREN { zend_do_pop_object(&$1 TSRMLS_CC); zend_do_begin_method_call(&$1 TSRMLS_CC); }
//				function_call_parameter_list RPAREN
//			{ zend_do_end_function_call(&$1, &$$, &$3, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
//;

method_i(A) ::= LPAREN(B). { zend_do_pop_object(&B TSRMLS_CC); zend_do_begin_method_call(&B TSRMLS_CC); A = B; }
method(A) ::= method_i(B) function_call_parameter_list(C) RPAREN. { zend_do_end_function_call(&B, &A, &C, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }

//method_or_not:			
//		method						{ $$ = $1; zend_do_push_object(&$$ TSRMLS_CC); $$.EA = ZEND_PARSED_METHOD_CALL; }
//	|	array_method_dereference	{ $$ = $1; zend_do_push_object(&$$ TSRMLS_CC); }
//	|	/* empty */ { $$.EA = ZEND_PARSED_MEMBER; }
//;

method_or_not(A) ::= . { A.EA = ZEND_PARSED_MEMBER; }
method_or_not(A) ::= method(B). { A = B; zend_do_push_object(&A TSRMLS_CC); A.EA = ZEND_PARSED_METHOD_CALL; }
method_or_not(A) ::= array_method_dereference(B). { A = B; zend_do_push_object(&A TSRMLS_CC); }

//variable_without_objects:
//		reference_variable { $$ = $1; }
//	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); }
//;

variable_without_objects(A) ::= reference_variable(B). { A = B; }
variable_without_objects(A) ::= simple_indirect_reference(B) reference_variable(C). { zend_do_indirect_references(&A, &B, &C TSRMLS_CC); }

//static_member:
//		class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM variable_without_objects { $$ = $3; zend_do_fetch_static_member(&$$, &$1 TSRMLS_CC); }
//;

static_member(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C). { A = C; zend_do_fetch_static_member(&A, &B TSRMLS_CC); }
static_member(A) ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C). { A = C; zend_do_fetch_static_member(&A, &B TSRMLS_CC); }

//variable_class_name:
//		reference_variable { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); $$=$1;; }
//;

variable_class_name(A) ::= reference_variable(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); A = B; }

//array_function_dereference:
//		array_function_dereference '[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$.EA = ZEND_PARSED_FUNCTION_CALL; } 
//		'[' dim_offset ']' { fetch_array_dim(&$$, &$1, &$4 TSRMLS_CC); }
//;

array_function_dereference_i(A) ::= function_call(B). { zend_do_begin_variable_parse(TSRMLS_C); A = B; A.EA = ZEND_PARSED_FUNCTION_CALL; } 

array_function_dereference(A) ::= array_function_dereference(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
array_function_dereference(A) ::= array_function_dereference_i(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }

//base_variable_with_function_calls:
//		base_variable				{ $$ = $1; }
//	|	array_function_dereference	{ $$ = $1; }
//	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$ = $1; $$.EA = ZEND_PARSED_FUNCTION_CALL; }
//;

base_variable_with_function_calls(A) ::= base_variable(B).	{ A = B; }
base_variable_with_function_calls(A) ::= array_function_dereference(B).	{ A = B; }
base_variable_with_function_calls(A) ::= function_call(B). { zend_do_begin_variable_parse(TSRMLS_C); A = B; A.EA = ZEND_PARSED_FUNCTION_CALL; }

//base_variable:
//		reference_variable { $$ = $1; $$.EA = ZEND_PARSED_VARIABLE; }
//	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); $$.EA = ZEND_PARSED_VARIABLE; }
//	|	static_member { $$ = $1; $$.EA = ZEND_PARSED_STATIC_MEMBER; }
//;

base_variable(A) ::= reference_variable(B). { A = B; A.EA = ZEND_PARSED_VARIABLE; }
base_variable(A) ::= simple_indirect_reference(B) reference_variable(C). { zend_do_indirect_references(&A, &B, &C TSRMLS_CC); A.EA = ZEND_PARSED_VARIABLE; }
base_variable(A) ::= static_member(B). { A = B; A.EA = ZEND_PARSED_STATIC_MEMBER; }

//reference_variable:
//		reference_variable '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	reference_variable LBRACE expr RBRACE		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
//	|	compound_variable			{ zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//;
//

reference_variable(A) ::= reference_variable(B) LBRACKET dim_offset(C) RBRACKET.	{ fetch_array_dim(&A, &B, &C TSRMLS_CC); }
reference_variable(A) ::= reference_variable(B) LBRACE expr(C) RBRACE.		{ fetch_string_offset(&A, &B, &C TSRMLS_CC); }
reference_variable(A) ::= compound_variable(B).	{ zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }

//
//compound_variable:
//		VARIABLE			{ $$ = $1; }
//	|	'$' LBRACE expr RBRACE	{ $$ = $3; }
//;

compound_variable(A) ::= VARIABLE(B). { A = B; }
compound_variable(A) ::= DOLLAR LBRACE expr(B) RBRACE.	{ A = B; }

//dim_offset:
//		/* empty */		{ $$.op_type = IS_UNUSED; }
//	|	expr			{ $$ = $1; }
//;

dim_offset(A) ::= . { A.op_type = IS_UNUSED; }
dim_offset(A) ::= expr(B).	{ A = B; }

//object_property:
//		object_dim_list { $$ = $1; }
//	|	variable_without_objects { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); } { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
//;

object_property(A) ::= object_dim_list(B). { A = B; }
object_property(A) ::= variable_without_objects(B). {
	znode tmp_znode; 
	zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC);	
	zend_do_pop_object(&tmp_znode TSRMLS_CC);
	zend_do_fetch_property(&A, &tmp_znode, &B TSRMLS_CC);
}

//object_dim_list:
//		object_dim_list '[' dim_offset ']'	{ fetch_array_dim(&$$, &$1, &$3 TSRMLS_CC); }
//	|	object_dim_list LBRACE expr RBRACE		{ fetch_string_offset(&$$, &$1, &$3 TSRMLS_CC); }
//	|	variable_name { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
//;

object_dim_list(A) ::= object_dim_list(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
object_dim_list(A) ::= object_dim_list(B) LBRACE expr(C) RBRACE. { fetch_string_offset(&A, &B, &C TSRMLS_CC); }
object_dim_list(A) ::= variable_name(B). {
	znode tmp_znode;
	zend_do_pop_object(&tmp_znode TSRMLS_CC);
	zend_do_fetch_property(&A, &tmp_znode, &B TSRMLS_CC);
}

//variable_name:
//		STRING		{ $$ = $1; }
//	|	LBRACE expr RBRACE	{ $$ = $2; }
//;

variable_name(A) ::= STRING(B). { A = B; }
variable_name(A) ::= LBRACE expr(B) RBRACE. { A = B; }

//simple_indirect_reference:
//		'$' { Z_LVAL($$.u.constant) = 1; }
//	|	simple_indirect_reference '$' { Z_LVAL($$.u.constant)++; }
//;

simple_indirect_reference(A) ::= DOLLAR. { Z_LVAL(A.u.constant) = 1; }
simple_indirect_reference(A) ::= simple_indirect_reference DOLLAR. { Z_LVAL(A.u.constant)++; }

//assignment_list:
//		assignment_list COMMA assignment_list_element
//	|	assignment_list_element
//;

assignment_list ::=	assignment_list COMMA assignment_list_element.
assignment_list ::=	assignment_list_element.

//assignment_list_element:
//		variable								{ zend_do_add_list_element(&$1 TSRMLS_CC); }
//	|	LIST LPAREN { zend_do_new_list_begin(TSRMLS_C); } assignment_list RPAREN	{ zend_do_new_list_end(TSRMLS_C); }
//	|	/* empty */							{ zend_do_add_list_element(NULL TSRMLS_CC); }
//;

assignment_list_element_i ::= LIST LPAREN. { zend_do_new_list_begin(TSRMLS_C); }

assignment_list_element ::=	variable(B). { zend_do_add_list_element(&B TSRMLS_CC); }
assignment_list_element ::=	assignment_list_element_i assignment_list RPAREN. { zend_do_new_list_end(TSRMLS_C); }
assignment_list_element ::=	. { zend_do_add_list_element(NULL TSRMLS_CC); }

//array_pair_list:
//		/* empty */ { zend_do_init_array(&$$, NULL, NULL, 0 TSRMLS_CC); }
//	|	non_empty_array_pair_list possible_comma	{ $$ = $1; }
//;

array_pair_list(A) ::= . { zend_do_init_array(&A, NULL, NULL, 0 TSRMLS_CC); }
array_pair_list(A) ::= non_empty_array_pair_list(B) possible_comma.	{ A = B; }

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

non_empty_array_pair_list(A) ::= non_empty_array_pair_list(B) COMMA expr(C) DOUBLE_ARROW expr(D). { zend_do_add_array_element(&B, &D, &C, 0 TSRMLS_CC); A = B; }
non_empty_array_pair_list(A) ::= non_empty_array_pair_list(B) COMMA expr(C). { zend_do_add_array_element(&B, &C, NULL, 0 TSRMLS_CC); A = B; }
non_empty_array_pair_list(A) ::= expr(B) DOUBLE_ARROW expr(C). { zend_do_init_array(&A, &C, &B, 0 TSRMLS_CC); }
non_empty_array_pair_list(A) ::= expr(B). { zend_do_init_array(&A, &B, NULL, 0 TSRMLS_CC); }
non_empty_array_pair_list(A) ::= non_empty_array_pair_list(B) COMMA expr(C) DOUBLE_ARROW BW_AND w_variable(D). { zend_do_add_array_element(&B, &D, &C, 1 TSRMLS_CC); A = B; }
non_empty_array_pair_list(A) ::= non_empty_array_pair_list(B) COMMA BW_AND w_variable(C). { zend_do_add_array_element(&B, &C, NULL, 1 TSRMLS_CC); A = B; }
non_empty_array_pair_list(A) ::= expr(B) DOUBLE_ARROW BW_AND w_variable(C).	{ zend_do_init_array(&A, &C, &B, 1 TSRMLS_CC); }
non_empty_array_pair_list(A) ::= BW_AND w_variable(B). { zend_do_init_array(&A, &B, NULL, 1 TSRMLS_CC); }

//encaps_list:
//		encaps_list encaps_var { zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&$$, &$1, &$2 TSRMLS_CC); }
//	|	encaps_list ENCAPSED_AND_WHITESPACE	{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
//	|	encaps_var { zend_do_end_variable_parse(&$1, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, NULL, &$1 TSRMLS_CC); }
//	|	ENCAPSED_AND_WHITESPACE encaps_var	{ zend_do_add_string(&$$, NULL, &$1 TSRMLS_CC); zend_do_end_variable_parse(&$2, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&$$, &$$, &$2 TSRMLS_CC); }
//;

encaps_list(A) ::= encaps_list(B) encaps_var(C). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&A, &B, &C TSRMLS_CC); }
encaps_list(A) ::= encaps_list(B) ENCAPSED_AND_WHITESPACE(C). { zend_do_add_string(&A, &B, &C TSRMLS_CC); }
encaps_list(A) ::= encaps_var(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&A, NULL, &B TSRMLS_CC); }
encaps_list(A) ::= ENCAPSED_AND_WHITESPACE(B) encaps_var(C). { zend_do_add_string(&A, NULL, &B TSRMLS_CC); zend_do_end_variable_parse(&C, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&A, &A, &C TSRMLS_CC); }

//encaps_var:
//		VARIABLE { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//	|	VARIABLE '[' { zend_do_begin_variable_parse(TSRMLS_C); } encaps_var_offset ']'	{ fetch_array_begin(&$$, &$1, &$4 TSRMLS_CC); }
//	|	VARIABLE OBJECT_OPERATOR STRING { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&$2, &$1, 1 TSRMLS_CC); zend_do_fetch_property(&$$, &$2, &$3 TSRMLS_CC); }
//	|	DOLLAR_OPEN_CURLY_BRACES expr RBRACE { zend_do_begin_variable_parse(TSRMLS_C);  fetch_simple_variable(&$$, &$2, 1 TSRMLS_CC); }
//	|	DOLLAR_OPEN_CURLY_BRACES STRING_VARNAME '[' expr ']' RBRACE { zend_do_begin_variable_parse(TSRMLS_C);  fetch_array_begin(&$$, &$2, &$4 TSRMLS_CC); }
//	|	CURLY_OPEN variable RBRACE { $$ = $2; }
//;

encaps_var_i(A) ::= VARIABLE(B) LBRACKET. { zend_do_begin_variable_parse(TSRMLS_C); A = B; }

encaps_var(A) ::= VARIABLE(B). { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }
encaps_var(A) ::= encaps_var_i(B) encaps_var_offset(C) RBRACKET. { fetch_array_begin(&A, &B, &C TSRMLS_CC); }
encaps_var(A) ::= VARIABLE(B) OBJECT_OPERATOR(C) STRING(D). { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&C, &B, 1 TSRMLS_CC); zend_do_fetch_property(&A, &C, &D TSRMLS_CC); }
encaps_var(A) ::= DOLLAR_OPEN_CURLY_BRACES expr(B) RBRACE. { zend_do_begin_variable_parse(TSRMLS_C);  fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }
encaps_var(A) ::= DOLLAR_OPEN_CURLY_BRACES STRING_VARNAME(B) LBRACKET expr(C) RBRACKET RBRACE. { zend_do_begin_variable_parse(TSRMLS_C);  fetch_array_begin(&A, &B, &C TSRMLS_CC); }
encaps_var(A) ::= CURLY_OPEN variable(B) RBRACE. { A = B; }

//encaps_var_offset:
//		STRING		{ $$ = $1; }
//	|	NUM_STRING	{ $$ = $1; }
//	|	VARIABLE		{ fetch_simple_variable(&$$, &$1, 1 TSRMLS_CC); }
//;

encaps_var_offset(A) ::= STRING(B). { A = B; }
encaps_var_offset(A) ::= NUM_STRING(B). { A = B; }
encaps_var_offset(A) ::= VARIABLE(B). { fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }

//internal_functions_in_yacc:
//		ISSET LPAREN isset_variables RPAREN { $$ = $3; }
//	|	EMPTY LPAREN variable RPAREN	{ zend_do_isset_or_isempty(ZEND_ISEMPTY, &$$, &$3 TSRMLS_CC); }
//	|	INCLUDE expr 			{ zend_do_include_or_eval(ZEND_INCLUDE, &$$, &$2 TSRMLS_CC); }
//	|	INCLUDE_ONCE expr 	{ zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &$$, &$2 TSRMLS_CC); }
//	|	EVAL LPAREN expr RPAREN 	{ zend_do_include_or_eval(ZEND_EVAL, &$$, &$3 TSRMLS_CC); }
//	|	REQUIRE expr			{ zend_do_include_or_eval(ZEND_REQUIRE, &$$, &$2 TSRMLS_CC); }
//	|	REQUIRE_ONCE expr		{ zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &$$, &$2 TSRMLS_CC); }
//;

internal_functions_in_yacc(A) ::= ISSET LPAREN isset_variables(B) RPAREN. { A = B; }
internal_functions_in_yacc(A) ::= EMPTY LPAREN variable(B) RPAREN. { zend_do_isset_or_isempty(ZEND_ISEMPTY, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= INCLUDE expr(B).  { zend_do_include_or_eval(ZEND_INCLUDE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= INCLUDE_ONCE expr(B). { zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= EVAL LPAREN expr(B) RPAREN.	{ zend_do_include_or_eval(ZEND_EVAL, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= REQUIRE expr(B). { zend_do_include_or_eval(ZEND_REQUIRE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= REQUIRE_ONCE expr(B). { zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &A, &B TSRMLS_CC); }

//isset_variables:
//		variable 				{ zend_do_isset_or_isempty(ZEND_ISSET, &$$, &$1 TSRMLS_CC); }
//	|	isset_variables COMMA { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } variable { znode tmp; zend_do_isset_or_isempty(ZEND_ISSET, &tmp, &$4 TSRMLS_CC); zend_do_boolean_and_end(&$$, &$1, &tmp, &$2 TSRMLS_CC); }
//;

isset_variables_i(A) ::= isset_variables(B) COMMA(C). { zend_do_boolean_and_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }

isset_variables(A) ::= variable(B).	{ zend_do_isset_or_isempty(ZEND_ISSET, &A, &B TSRMLS_CC); }
isset_variables(A) ::= isset_variables_i(B) variable(C). { 
	znode tmp; zend_do_isset_or_isempty(ZEND_ISSET, &tmp, &C TSRMLS_CC);
	zend_do_boolean_and_end(&A, &B[0], &tmp, &B[1] TSRMLS_CC);
}

//class_constant:
//		class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
//	|	variable_class_name PAAMAYIM_NEKUDOTAYIM STRING { zend_do_fetch_constant(&$$, &$1, &$3, ZEND_RT, 0 TSRMLS_CC); }
//;

class_constant(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C). { zend_do_fetch_constant(&A, &B, &C, ZEND_RT, 0 TSRMLS_CC); }
class_constant(A) ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C). { zend_do_fetch_constant(&A, &B, &C, ZEND_RT, 0 TSRMLS_CC); }


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
