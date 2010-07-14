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
   | Authors: Felipe Pena <felipe@php.net>                                |
   |          Etienne Kneuss <colder@php.net>                             |
   | Yacc version authors:                                                |
   |          Andi Gutmans <andi@zend.com>                                |
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

#define YYNOERRORRECOVERY
#define NDEBUG
}

%name zend_lang_parse

%syntax_error {
	if (LANG_SCNG(yy_cursor) > LANG_SCNG(yy_limit)) {
		zend_error(E_PARSE, "syntax error, unexpected end of script");
	} else {
		zend_error(E_PARSE, "syntax error, unexpected '%.*s'", LANG_SCNG(yy_leng), LANG_SCNG(yy_text));
	}
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


/* Precedences */
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


/* Productions */
start ::= top_statement_list. { zend_do_end_compilation(TSRMLS_C); }

top_statement_list_i ::= top_statement_list. { zend_do_extended_info(TSRMLS_C); }

top_statement_list ::= top_statement_list_i top_statement. { HANDLE_INTERACTIVE(); }
top_statement_list ::= .


namespace_name(A) ::= STRING(B). { A = B; }
namespace_name(A) ::= namespace_name(B) NS_SEPARATOR STRING(C). { zend_do_build_namespace_name(&A, &B, &C TSRMLS_CC); }

namespace_start ::= NAMESPACE. { zend_do_begin_namespace(NULL, 1 TSRMLS_CC); }

namespace_start2 ::= NAMESPACE namespace_name(B). { zend_do_begin_namespace(&B, 1 TSRMLS_CC); }

top_statement ::= SEMICOLON.                                 { zend_do_ticks(TSRMLS_C); }
top_statement ::= statement.                                 { zend_verify_namespace(TSRMLS_C); }
top_statement ::= function_declaration_statement.            { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
top_statement ::= class_declaration_statement.               { zend_verify_namespace(TSRMLS_C); zend_do_early_binding(TSRMLS_C); }
top_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON.     { zend_do_halt_compiler_register(TSRMLS_C); }
top_statement ::= NAMESPACE namespace_name(B) SEMICOLON.     { zend_do_begin_namespace(&B, 0 TSRMLS_CC); }
top_statement ::= namespace_start2 LBRACE top_statement_list RBRACE. { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= namespace_start  LBRACE top_statement_list RBRACE.   { zend_do_end_namespace(TSRMLS_C); }
top_statement ::= USE use_declarations SEMICOLON.            { zend_verify_namespace(TSRMLS_C); }
top_statement ::= constant_declaration SEMICOLON.            { zend_verify_namespace(TSRMLS_C); }

use_declarations ::= use_declarations COMMA use_declaration.
use_declarations ::= use_declaration.


use_declaration ::= namespace_name(B).                           { zend_do_use(&B, NULL, 0 TSRMLS_CC); }
use_declaration ::= namespace_name(B) AS STRING(C).              { zend_do_use(&B, &C, 0 TSRMLS_CC); }
use_declaration ::= NS_SEPARATOR namespace_name(B).              { zend_do_use(&B, NULL, 1 TSRMLS_CC); }
use_declaration ::= NS_SEPARATOR namespace_name(B) AS STRING(C). { zend_do_use(&B, &C, 1 TSRMLS_CC); }


constant_declaration ::= constant_declaration COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_constant(&B, &C TSRMLS_CC); }
constant_declaration ::= CONST STRING(B) EQUAL static_scalar(C).                      { zend_do_declare_constant(&B, &C TSRMLS_CC); }

inner_statement_list_i ::= inner_statement_list. { zend_do_extended_info(TSRMLS_C); }

inner_statement_list ::= inner_statement_list_i inner_statement. { HANDLE_INTERACTIVE(); }
inner_statement_list ::= .


inner_statement ::= statement.
inner_statement ::= function_declaration_statement.
inner_statement ::= class_declaration_statement.
inner_statement ::= HALT_COMPILER LPAREN RPAREN SEMICOLON. { zend_error(E_COMPILE_ERROR, "__HALT_COMPILER() can only be used from the outermost scope"); }

statement ::= unticked_statement.  { zend_do_ticks(TSRMLS_C); }
statement ::= STRING(B) COLON.     { zend_do_label(&B TSRMLS_CC); }



elseif_list_i(A) ::= elseif_list ELSEIF LPAREN expr(B) RPAREN(C). { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }

elseif_list ::= .
elseif_list ::= elseif_list_i(B) statement. { zend_do_if_after_statement(&B, 0 TSRMLS_CC); }

else ::= .
else ::= ELSE statement.

if_cond(A)   ::= IF LPAREN expr(B) RPAREN(C). { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }
if_cond_then ::= if_cond(B) statement.        { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

if ::= if_cond_then elseif_list else. { zend_do_if_end(TSRMLS_C); } 

elseif_alt_list_i(A) ::= elseif_alt_list ELSEIF LPAREN expr(B) RPAREN(C) COLON. { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }

elseif_alt_list ::= .
elseif_alt_list ::= elseif_alt_list_i(B) inner_statement_list. { zend_do_if_after_statement(&B, 0 TSRMLS_CC); }

else_alt ::= .
else_alt ::= ELSE COLON inner_statement_list.

if_alt_cond(A)   ::= IF LPAREN expr(B) RPAREN(C) COLON.   { zend_do_if_cond(&B, &C TSRMLS_CC); A = C; }
if_alt_cond_then ::= if_alt_cond(B) inner_statement_list. { zend_do_if_after_statement(&B, 1 TSRMLS_CC); }

if_alt ::= if_alt_cond_then elseif_alt_list else_alt ENDIF SEMICOLON. { zend_do_if_end(TSRMLS_C); }

while_statement ::= statement.
while_statement ::= COLON inner_statement_list ENDWHILE SEMICOLON.

while_begin(A) ::= WHILE(B) LPAREN.                   { B.u.op.opline_num = get_next_op_number(CG(active_op_array)); A = B; }
while_cond(A)  ::= while_begin(B)  expr(C) RPAREN(D). { zend_do_while_cond(&C, &D TSRMLS_CC); A[0] = B; A[1] = D; }
while          ::= while_cond(B) while_statement.     { zend_do_while_end(&B[0], &B[1] TSRMLS_CC); }


do_start(A)     ::= DO. { A.u.op.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); }
do_statement(A) ::= do_start(B) statement WHILE LPAREN(C). { C.u.op.opline_num = get_next_op_number(CG(active_op_array)); A[0] = B; A[1] = C; }
do_while        ::= do_statement(B) expr(C) RPAREN SEMICOLON. { zend_do_do_while_end(&B[0], &B[1], &C TSRMLS_CC); }

for_expr(A) ::= .                      { A.op_type = IS_CONST;  Z_TYPE(A.u.constant) = IS_BOOL;  Z_LVAL(A.u.constant) = 1; }
for_expr(A) ::= non_empty_for_expr(B). { A = B; }

non_empty_for_expr_i ::= non_empty_for_expr(B) COMMA. { zend_do_free(&B TSRMLS_CC); }

non_empty_for_expr(A) ::= non_empty_for_expr_i expr(B). { A = B; }
non_empty_for_expr(A) ::= expr(B). { A = B; }

for_statement ::= statement.
for_statement ::= COLON inner_statement_list ENDFOR SEMICOLON.

for_begin(A)  ::= FOR LPAREN for_expr(B) SEMICOLON(C). { zend_do_free(&B TSRMLS_CC); C.u.op.opline_num = get_next_op_number(CG(active_op_array)); A = C; }
for_cont(A)   ::= for_begin(B) for_expr(C) SEMICOLON(D). { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&C, &D TSRMLS_CC); A[0] = B; A[1] = D; }
for_cont_2(A) ::= for_cont(B) for_expr(C) RPAREN. { zend_do_free(&C TSRMLS_CC); zend_do_for_before_statement(&B[0], &B[1] TSRMLS_CC); A = B[1]; }
for           ::= for_cont_2(B) for_statement.             { zend_do_for_end(&B TSRMLS_CC); }


switch_case_list(A) ::= LBRACE case_list(B) RBRACE.                       { A = B; }
switch_case_list(A) ::= LBRACE SEMICOLON case_list(B) RBRACE.             { A = B; }
switch_case_list(A) ::= COLON case_list(B) ENDSWITCH SEMICOLON.           { A = B; }
switch_case_list(A) ::= COLON SEMICOLON case_list(B) ENDSWITCH SEMICOLON. { A = B; }


case_list_i(A)  ::= case_list(B) CASE(C) expr(D) case_separator. { zend_do_extended_info(TSRMLS_C);  zend_do_case_before_statement(&B, &C, &D TSRMLS_CC); A = C;}
case_list_ii(A) ::= case_list(B) DEFAULT(C) case_separator. { zend_do_extended_info(TSRMLS_C);  zend_do_default_before_statement(&B, &C TSRMLS_CC); A = C; }

case_list(A) ::= . { A.op_type = IS_UNUSED; }
case_list(A) ::= case_list_i(B) inner_statement_list. { zend_do_case_after_statement(&A, &B TSRMLS_CC); A.op_type = IS_CONST; }
case_list(A) ::= case_list_ii(B) inner_statement_list. { zend_do_case_after_statement(&A, &B TSRMLS_CC); A.op_type = IS_CONST; }

case_separator ::= COLON.
case_separator ::= SEMICOLON.
switch_i ::=  SWITCH LPAREN expr(B) RPAREN. { zend_do_switch_cond(&B TSRMLS_CC); }
switch   ::= switch_i switch_case_list(B).  { zend_do_switch_end(&B TSRMLS_CC); }

declare_statement ::= statement.
declare_statement ::= COLON inner_statement_list ENDDECLARE SEMICOLON.

declare_list ::= STRING(B) EQUAL static_scalar(C).                    { zend_do_declare_stmt(&B, &C TSRMLS_CC); }
declare_list ::= declare_list COMMA STRING(B) EQUAL static_scalar(C). { zend_do_declare_stmt(&B, &C TSRMLS_CC); }

declare_i(A) ::= DECLARE(B). { A.u.op.opline_num = get_next_op_number(CG(active_op_array)); zend_do_declare_begin(TSRMLS_C); A = B; }
declare      ::= declare_i(B) LPAREN declare_list RPAREN declare_statement. { zend_do_declare_end(&B TSRMLS_CC); }


foreach_optional_arg(A) ::= .                                 { A.op_type = IS_UNUSED; }
foreach_optional_arg(A) ::= DOUBLE_ARROW foreach_variable(B). { A = B; }

foreach_variable(A) ::= variable(B).        { zend_check_writable_variable(&B); A = B; }
foreach_variable(A) ::= BW_AND variable(B). { zend_check_writable_variable(&B); A = B; A.EA |= ZEND_PARSED_REFERENCE_VARIABLE; }

foreach_statement ::= statement.
foreach_statement ::= COLON inner_statement_list ENDFOREACH SEMICOLON.

foreach_ii(A) ::= FOREACH(B) LPAREN(C) variable(D) AS(E). { zend_do_foreach_begin(&B, &C, &D, &E, 1 TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }
foreach_i(A)  ::= foreach_ii(B) foreach_variable(C) foreach_optional_arg(D) RPAREN. { zend_do_foreach_cont(&B[0], &B[1], &B[2], &C, &D TSRMLS_CC); A[0] = B[0]; A[1] = B[2]; }
foreach       ::= foreach_i(B) foreach_statement. { zend_do_foreach_end(&B[0], &B[1] TSRMLS_CC); }

foreach2_ii(A) ::= FOREACH(B) LPAREN(C) expr_wo_var(D) AS(E). { zend_do_foreach_begin(&B, &C, &D, &E, 0 TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }
foreach2_i(A)  ::= foreach2_ii(B) variable(C) foreach_optional_arg(D) RPAREN. { zend_check_writable_variable(&C); zend_do_foreach_cont(&B[0], &B[1], &B[2], &C, &D TSRMLS_CC); A[0] = B[0]; A[1] = B[2]; }
foreach2       ::= foreach2_i(B) foreach_statement. { zend_do_foreach_end(&B[0], &B[1] TSRMLS_CC); }


additional_catches(A) ::= . { A.u.op.opline_num = -1; }
additional_catches(A) ::= non_empty_additional_catches(B). { A = B; }

non_empty_additional_catches(A) ::= additional_catch(B). { A = B; }
non_empty_additional_catches(A) ::= non_empty_additional_catches additional_catch(B). { A = B; }

additional_catch_i(A)  ::= CATCH(B) LPAREN fully_qualified_class_name(C). { A[0] = B; A[1] = C; A[0].u.op.opline_num = get_next_op_number(CG(active_op_array)); }
additional_catch_ii(A) ::= additional_catch_i(B) VARIABLE(C) RPAREN. { zend_do_begin_catch(&B[0], &B[1], &C, NULL TSRMLS_CC); A = B[0]; }
additional_catch       ::= additional_catch_ii(B) LBRACE inner_statement_list RBRACE. { zend_do_end_catch(&B TSRMLS_CC); }
try_catch_v(A)   ::= TRY(B). { zend_do_try(&B TSRMLS_CC); A = B; }
try_catch_iv(A)  ::= try_catch_v(B) LBRACE inner_statement_list RBRACE CATCH LPAREN(C). { zend_initialize_try_catch_element(&B TSRMLS_CC); A[0] = B; A[1] = C; }
try_catch_iii(A) ::= try_catch_iv(B) fully_qualified_class_name(C). { zend_do_first_catch(&B[1] TSRMLS_CC); A[0] = B[0]; A[1] = B[1]; A[2] = C; }
try_catch_ii(A)  ::= try_catch_iii(B) VARIABLE(C) RPAREN. { zend_do_begin_catch(&B[0], &B[2], &C, &B[1] TSRMLS_CC); A = B[0]; }
try_catch_i(A)   ::= try_catch_ii(B) LBRACE inner_statement_list RBRACE. { zend_do_end_catch(&B TSRMLS_CC); A = B; }
try_catch        ::= try_catch_i(B) additional_catches(C). { zend_do_mark_last_catch(&B, &C TSRMLS_CC); }

unticked_statement ::= if.
unticked_statement ::= if_alt.
unticked_statement ::= switch.
unticked_statement ::= do_while.
unticked_statement ::= for.
unticked_statement ::= while.
unticked_statement ::= foreach.
unticked_statement ::= foreach2.
unticked_statement ::= declare.
unticked_statement ::= try_catch.
unticked_statement ::= LBRACE inner_statement_list RBRACE.
unticked_statement ::= GLOBAL global_var_list SEMICOLON.
unticked_statement ::= STATIC static_var_list SEMICOLON.
unticked_statement ::= ECHO echo_expr_list SEMICOLON.
unticked_statement ::= UNSET LPAREN unset_variables RPAREN SEMICOLON.
unticked_statement ::= SEMICOLON. /* empty statement */
unticked_statement ::= BREAK SEMICOLON.                               { zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
unticked_statement ::= BREAK expr(B) SEMICOLON.                       { zend_do_brk_cont(ZEND_BRK, &B TSRMLS_CC); }
unticked_statement ::= CONTINUE SEMICOLON.                            { zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
unticked_statement ::= CONTINUE expr(B) SEMICOLON.                    { zend_do_brk_cont(ZEND_CONT, &B TSRMLS_CC); }
unticked_statement ::= RETURN SEMICOLON.                              { zend_do_return(NULL, 0 TSRMLS_CC); }
unticked_statement ::= RETURN expr_wo_var(B) SEMICOLON.     { zend_do_return(&B, 0 TSRMLS_CC); }
unticked_statement ::= RETURN variable(B) SEMICOLON.                  { zend_do_return(&B, 1 TSRMLS_CC); }
unticked_statement ::= INLINE_HTML(B).                                { zend_do_echo(&B TSRMLS_CC); }
unticked_statement ::= expr(B) SEMICOLON.                             { zend_do_free(&B TSRMLS_CC); }
unticked_statement ::= THROW expr(B) SEMICOLON.                       { zend_do_throw(&B TSRMLS_CC); }
unticked_statement ::= GOTO STRING(B) SEMICOLON.                      { zend_do_goto(&B TSRMLS_CC); }

unset_variables ::= unset_variable.
unset_variables ::= unset_variables COMMA unset_variable.

unset_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&B TSRMLS_CC); A = B; }

function_declaration_statement ::= unticked_function_declaration_statement. { zend_do_ticks(TSRMLS_C); }

class_declaration_statement ::= unticked_class_declaration_statement. { zend_do_ticks(TSRMLS_C); }

is_reference(A) ::= . { A.op_type = ZEND_RETURN_VAL; }
is_reference(A) ::= BW_AND. { A.op_type = ZEND_RETURN_REF; }

unticked_function_declaration_statement_i(A) ::= function(B) is_reference(C) STRING(D). { zend_do_begin_function_declaration(&B, &D, 0, C.op_type, NULL TSRMLS_CC); A = B; }
unticked_function_declaration_statement ::= unticked_function_declaration_statement_i(B) LPAREN parameters RPAREN LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B TSRMLS_CC); }


unticked_class_declaration_statement_i(A) ::= class_entry_type(B) STRING(C) extends_from(D). { zend_do_begin_class_declaration(&B, &C, &D TSRMLS_CC); A[0] = B; A[1] = C; }
unticked_class_declaration_statement_ii(A) ::= interface_entry(B) STRING(C). { zend_do_begin_class_declaration(&B, &C, NULL TSRMLS_CC); A[0] = B; A[1] = C; }

unticked_class_declaration_statement ::= unticked_class_declaration_statement_i(B)
                                         implements_list
                                         LBRACE
                                             class_statements
                                         RBRACE. { zend_do_end_class_declaration(&B[0], &B[1] TSRMLS_CC); }

unticked_class_declaration_statement ::= unticked_class_declaration_statement_ii(B)
                                         interface_extends_list
                                         LBRACE
                                             class_statements
                                         RBRACE. { zend_do_end_class_declaration(&B[0], &B[1] TSRMLS_CC); }


class_entry_type(A) ::= CLASS.          { A.u.op.opline_num = CG(zend_lineno); A.EA = 0; }
class_entry_type(A) ::= ABSTRACT CLASS. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
class_entry_type(A) ::= TRAIT.          { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_TRAIT; }
class_entry_type(A) ::= FINAL CLASS.    { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_FINAL_CLASS; }


extends_from(A) ::= .                                      { A.op_type = IS_UNUSED; }
extends_from(A) ::= EXTENDS fully_qualified_class_name(B). { zend_do_fetch_class(&A, &B TSRMLS_CC); }

interface_entry(A) ::= INTERFACE. { A.u.op.opline_num = CG(zend_lineno); A.EA = ZEND_ACC_INTERFACE; }

interface_extends_list ::= .
interface_extends_list ::= EXTENDS interface_list.

implements_list ::= .
implements_list ::= IMPLEMENTS interface_list.

interface_list ::= fully_qualified_class_name(B). { zend_do_implements_interface(&B TSRMLS_CC); }
interface_list ::= interface_list COMMA fully_qualified_class_name(B). { zend_do_implements_interface(&B TSRMLS_CC); }

parameters ::= .
parameters ::= parameters_list.

parameters_list(A) ::= type_hint(B) VARIABLE(C).                               { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &C, &A, NULL, &B, 0 TSRMLS_CC); }
parameters_list(A) ::= type_hint(B) BW_AND VARIABLE(C).                        { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV, &C, &A, NULL, &B, 1 TSRMLS_CC); }
parameters_list(A) ::= type_hint(B) BW_AND VARIABLE(C) EQUAL static_scalar(D). { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &C, &A, &D, &B, 1 TSRMLS_CC); }
parameters_list(A) ::= type_hint(B) VARIABLE(C) EQUAL static_scalar(D).        { A.op_type = IS_UNUSED; A.u.op.num=1; zend_do_receive_arg(ZEND_RECV_INIT, &C, &A, &D, &B, 0 TSRMLS_CC); }

parameters_list(A) ::= parameters_list(B) COMMA type_hint(C) VARIABLE(D).                               { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV, &D, &A, NULL, &C, 0 TSRMLS_CC); }
parameters_list(A) ::= parameters_list(B) COMMA type_hint(C) BW_AND VARIABLE(D).                        { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV, &D, &A, NULL, &C, 1 TSRMLS_CC); }
parameters_list(A) ::= parameters_list(B) COMMA type_hint(C) BW_AND VARIABLE(D) EQUAL static_scalar(E). { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &D, &A, &E, &C, 1 TSRMLS_CC); }
parameters_list(A) ::= parameters_list(B) COMMA type_hint(C) VARIABLE(D) EQUAL static_scalar(E).        { A = B; A.u.op.num++; zend_do_receive_arg(ZEND_RECV_INIT, &D, &A, &E, &C, 0 TSRMLS_CC); }

type_hint(A) ::= .                              { A.op_type = IS_UNUSED; }
type_hint(A) ::= ARRAY(B).                      { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_ARRAY; }
type_hint(A) ::= BOOL_HINT(B).                  { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_BOOL; }
type_hint(A) ::= STRING_HINT(B).                { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_STRING; }
type_hint(A) ::= INT_HINT(B).                   { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_LONG; }
type_hint(A) ::= DOUBLE_HINT(B).                { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_DOUBLE; }
type_hint(A) ::= RESOURCE_HINT(B).              { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_RESOURCE; }
type_hint(A) ::= OBJECT_HINT(B).                { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_OBJECT; }
type_hint(A) ::= SCALAR_HINT(B).                { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_SCALAR; }
type_hint(A) ::= NUMERIC_HINT(B).               { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_NUMERIC; }
type_hint(A) ::= fully_qualified_class_name(B). { A = B; A.op_type = IS_CONST; Z_TYPE(A.u.constant) = IS_CLASS; }

fcall_parameters(A) ::= . { Z_LVAL(A.u.constant) = 0; }
fcall_parameters(A) ::= fcall_parameters_list(B). { A = B; }

fcall_parameters_list(A) ::= expr_wo_var(B).                                { Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_VAL, Z_LVAL(A.u.constant) TSRMLS_CC); }
fcall_parameters_list(A) ::= variable(B).                                             { Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_VAR, Z_LVAL(A.u.constant) TSRMLS_CC); }
fcall_parameters_list(A) ::= BW_AND w_variable(B).                                    { Z_LVAL(A.u.constant) = 1;  zend_do_pass_param(&B, ZEND_SEND_REF, Z_LVAL(A.u.constant) TSRMLS_CC); }
fcall_parameters_list(A) ::= fcall_parameters_list(B) COMMA expr_wo_var(C). { Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_VAL, Z_LVAL(A.u.constant) TSRMLS_CC); }
fcall_parameters_list(A) ::= fcall_parameters_list(B) COMMA variable(C).              { Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_VAR, Z_LVAL(A.u.constant) TSRMLS_CC); }
fcall_parameters_list(A) ::= fcall_parameters_list(B) COMMA BW_AND w_variable(C).     { Z_LVAL(A.u.constant)=Z_LVAL(B.u.constant)+1;  zend_do_pass_param(&C, ZEND_SEND_REF, Z_LVAL(A.u.constant) TSRMLS_CC); }

global_var_list ::= global_var_list COMMA global_var(B). { zend_do_fetch_global_variable(&B, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }
global_var_list ::= global_var(B).                       { zend_do_fetch_global_variable(&B, NULL, ZEND_FETCH_GLOBAL_LOCK TSRMLS_CC); }

global_var(A) ::= VARIABLE(B).                  { A = B; }
global_var(A) ::= DOLLAR r_variable(B).         { A = B; }
global_var(A) ::= DOLLAR LBRACE expr(B) RBRACE. { A = B; }

static_var_list ::= static_var_list COMMA VARIABLE(B).                        { zend_do_fetch_static_variable(&B, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= static_var_list COMMA VARIABLE(B) EQUAL static_scalar(C). { zend_do_fetch_static_variable(&B, &C, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= VARIABLE(B).                                              { zend_do_fetch_static_variable(&B, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
static_var_list ::= VARIABLE(B) EQUAL static_scalar(C).                       { zend_do_fetch_static_variable(&B, &C, ZEND_FETCH_STATIC TSRMLS_CC); }

class_statements ::= .
class_statements ::= class_statements class_statement.


class_statement_i     ::= variable_modifiers(B).                                     { CG(access_type) = Z_LVAL(B.u.constant); }
class_statement_ii(A) ::= method_modifiers(B) function(C) is_reference(D) STRING(E). { zend_do_begin_function_declaration(&C, &E, 1, D.op_type, &B TSRMLS_CC); A[0] = B; A[1] = C; A[2] = E; }

class_statement ::= class_statement_i class_variables SEMICOLON.
class_statement ::= class_constants SEMICOLON.
class_statement ::= trait_use_statement.
class_statement ::= class_statement_ii(B) LPAREN parameters RPAREN method_body(C). { zend_do_abstract_method(&B[2], &B[0], &C TSRMLS_CC); zend_do_end_function_declaration(&B[1] TSRMLS_CC); }


trait_use_statement ::= USE trait_list trait_adaptations.

trait_list ::= fully_qualified_class_name(B).                   { zend_do_implements_trait(&B TSRMLS_CC); }
trait_list ::= trait_list COMMA fully_qualified_class_name(B).  { zend_do_implements_trait(&B TSRMLS_CC); }

trait_adaptations ::= SEMICOLON.
trait_adaptations ::= LBRACE trait_adaptations_list RBRACE.

trait_adaptations_list ::= .
trait_adaptations_list ::= trait_adaptations_list trait_adaptation_statement.

trait_adaptation_statement ::= trait_precedence(B) SEMICOLON. { zend_add_trait_precedence(&B TSRMLS_CC); }
trait_adaptation_statement ::= trait_alias(B) SEMICOLON.      { zend_add_trait_alias(&B TSRMLS_CC); }

trait_precedence(A) ::= trait_method_reference_fully_qualified(B) INSTEADOF trait_reference_list(C). { zend_prepare_trait_precedence(&A, &B, &C TSRMLS_CC); }

trait_reference_list(A) ::= fully_qualified_class_name(B).                               { zend_init_list(&A.u.op.ptr, Z_STRVAL(B.u.constant) TSRMLS_CC); }
trait_reference_list(A) ::= trait_reference_list(B) COMMA fully_qualified_class_name(C). { zend_add_to_list(&B.u.op.ptr, Z_STRVAL(C.u.constant) TSRMLS_CC); A = B; }


trait_method_reference(A) ::= STRING(B).                                 { zend_prepare_reference(&A, NULL, &B TSRMLS_CC); }
trait_method_reference(A) ::= trait_method_reference_fully_qualified(B). { A = B; }

trait_method_reference_fully_qualified(A) ::= fully_qualified_class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C).	{ zend_prepare_reference(&A, &B, &C TSRMLS_CC); }

trait_alias(A) ::= trait_method_reference(B) AS trait_modifiers(C) STRING(D). { zend_prepare_trait_alias(&A, &B, &C, &D TSRMLS_CC); }
trait_alias(A) ::= trait_method_reference(B) AS member_modifier(C).           { zend_prepare_trait_alias(&A, &B, &C, NULL TSRMLS_CC); }

trait_modifiers(A) ::= .                    { Z_LVAL(A.u.constant) = 0x0; } /* No change of methods visibility */
trait_modifiers(A) ::= member_modifier(B).  { A = B; } /* REM: Keep in mind, there are not only visibility modifiers */

method_body(A) ::= SEMICOLON.                           { Z_LVAL(A.u.constant) = ZEND_ACC_ABSTRACT; }
method_body(A) ::= LBRACE inner_statement_list RBRACE.  { Z_LVAL(A.u.constant) = 0;	}

variable_modifiers(A) ::= member_modifiers_list(B). { A = B; }
variable_modifiers(A) ::= VAR.                      { Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }

method_modifiers(A) ::= .                         { Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }
method_modifiers(A) ::= member_modifiers_list(B). { A = B;  if (!(Z_LVAL(A.u.constant) & ZEND_ACC_PPP_MASK)) { Z_LVAL(A.u.constant) |= ZEND_ACC_PUBLIC; } }

member_modifiers_list(A) ::= member_modifier(B).                          { A = B; }
member_modifiers_list(A) ::= member_modifiers_list(B) member_modifier(C). { Z_LVAL(A.u.constant) = zend_do_verify_access_types(&B, &C); }


member_modifier(A) ::= PUBLIC.      { Z_LVAL(A.u.constant) = ZEND_ACC_PUBLIC; }
member_modifier(A) ::= PROTECTED.   { Z_LVAL(A.u.constant) = ZEND_ACC_PROTECTED; }
member_modifier(A) ::= PRIVATE.     { Z_LVAL(A.u.constant) = ZEND_ACC_PRIVATE; }
member_modifier(A) ::= STATIC.      { Z_LVAL(A.u.constant) = ZEND_ACC_STATIC; }
member_modifier(A) ::= ABSTRACT.    { Z_LVAL(A.u.constant) = ZEND_ACC_ABSTRACT; }
member_modifier(A) ::= FINAL.       { Z_LVAL(A.u.constant) = ZEND_ACC_FINAL; }

class_variables ::= class_variables COMMA VARIABLE(B).                        { zend_do_declare_property(&B, NULL, CG(access_type) TSRMLS_CC); }
class_variables ::= class_variables COMMA VARIABLE(B) EQUAL static_scalar(C). { zend_do_declare_property(&B, &C, CG(access_type) TSRMLS_CC); }
class_variables ::= VARIABLE(B).                                              { zend_do_declare_property(&B, NULL, CG(access_type) TSRMLS_CC); }
class_variables ::= VARIABLE(B) EQUAL static_scalar(C).                       { zend_do_declare_property(&B, &C, CG(access_type) TSRMLS_CC); }

class_constants ::= class_constants COMMA STRING(B) EQUAL static_scalar(C).   { zend_do_declare_class_constant(&B, &C TSRMLS_CC); }
class_constants ::= CONST STRING(B) EQUAL static_scalar(C).                   { zend_do_declare_class_constant(&B, &C TSRMLS_CC); }

echo_expr_list ::= expr(B).                      { zend_do_echo(&B TSRMLS_CC); }
echo_expr_list ::= echo_expr_list COMMA expr(B). { zend_do_echo(&B TSRMLS_CC); }

new_i(A)  ::= NEW(B) class_name_reference(C). { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&B, &C TSRMLS_CC); A = B; } 
new_ii(A) ::= variable(B) EQUAL BW_AND(C) new_i(D). {
    zend_error(E_DEPRECATED, "Assigning the return value of new by reference is deprecated");
    zend_check_writable_variable(&B);
    A[0] = B;
    A[1] = C;
    A[2] = D;
}

list_i  ::= LIST LPAREN.                                 { zend_do_list_init(TSRMLS_C); }
list(A) ::= list_i assignment_list RPAREN EQUAL expr(B). { zend_do_list_end(&A, &B TSRMLS_CC); }

expr_wo_var(A) ::= list(B).                              { A = B; }
expr_wo_var(A) ::= variable(B) EQUAL expr(C).            { zend_check_writable_variable(&B); zend_do_assign(&A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) EQUAL BW_AND variable(C). { zend_check_writable_variable(&B); zend_do_end_variable_parse(&C, BP_VAR_W, 1 TSRMLS_CC); zend_do_end_variable_parse(&B, BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= new_ii(B) ctor_arguments(C).          { zend_do_end_new_object(&B[1], &B[2], &C TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(&B[0], BP_VAR_W, 0 TSRMLS_CC); B[1].EA = ZEND_PARSED_NEW; zend_do_assign_ref(&A, &B[0], &B[1] TSRMLS_CC); }
expr_wo_var(A) ::= new_i(B) ctor_arguments(C).           { zend_do_end_new_object(&A, &B, &C TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
expr_wo_var(A) ::= CLONE expr(B).                        { zend_do_clone(&A, &B TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) PLUS_EQUAL expr(C).       { zend_do_binary_assign_op(ZEND_ASSIGN_ADD, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) MINUS_EQUAL expr(C).      { zend_do_binary_assign_op(ZEND_ASSIGN_SUB, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) MUL_EQUAL expr(C).        { zend_do_binary_assign_op(ZEND_ASSIGN_MUL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) DIV_EQUAL expr(C).        { zend_do_binary_assign_op(ZEND_ASSIGN_DIV, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) CONCAT_EQUAL expr(C).     { zend_do_binary_assign_op(ZEND_ASSIGN_CONCAT, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) MOD_EQUAL expr(C).        { zend_do_binary_assign_op(ZEND_ASSIGN_MOD, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) AND_EQUAL expr(C).        { zend_do_binary_assign_op(ZEND_ASSIGN_BW_AND, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) OR_EQUAL expr(C).         { zend_do_binary_assign_op(ZEND_ASSIGN_BW_OR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) XOR_EQUAL expr(C).        { zend_do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) SL_EQUAL expr(C).         { zend_do_binary_assign_op(ZEND_ASSIGN_SL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= variable(B) SR_EQUAL expr(C).         { zend_do_binary_assign_op(ZEND_ASSIGN_SR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= rw_variable(B) INC.                   { zend_do_post_incdec(&A, &B, ZEND_POST_INC TSRMLS_CC); }
expr_wo_var(A) ::= INC rw_variable(B).                   { zend_do_pre_incdec(&A, &B, ZEND_PRE_INC TSRMLS_CC); }
expr_wo_var(A) ::= rw_variable(B) DEC.                   { zend_do_post_incdec(&A, &B, ZEND_POST_DEC TSRMLS_CC); }
expr_wo_var(A) ::= DEC rw_variable(B).                   { zend_do_pre_incdec(&A, &B, ZEND_PRE_DEC TSRMLS_CC); }
expr_wo_var(A) ::= bool_or_expr(B).                      { A = B; }
expr_wo_var(A) ::= bool_and_expr(B).                     { A = B; }
expr_wo_var(A) ::= logic_or_expr(B).                     { A = B; }
expr_wo_var(A) ::= logic_and_expr(B).                    { A = B; }
expr_wo_var(A) ::= expr(B) LOGICAL_XOR expr(C).          { zend_do_binary_op(ZEND_BOOL_XOR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) BW_OR expr(C).                { zend_do_binary_op(ZEND_BW_OR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) BW_AND expr(C).               { zend_do_binary_op(ZEND_BW_AND, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) BW_XOR expr(C).               { zend_do_binary_op(ZEND_BW_XOR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) DOT expr(C).                  { zend_do_binary_op(ZEND_CONCAT, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) PLUS expr(C).                 { zend_do_binary_op(ZEND_ADD, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) MINUS expr(C).                { zend_do_binary_op(ZEND_SUB, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) MULT expr(C).                 { zend_do_binary_op(ZEND_MUL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) DIV expr(C).                  { zend_do_binary_op(ZEND_DIV, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) MOD expr(C).                  { zend_do_binary_op(ZEND_MOD, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) SL expr(C).                   { zend_do_binary_op(ZEND_SL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) SR expr(C).                   { zend_do_binary_op(ZEND_SR, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= BOOL_NOT expr(B).                     { zend_do_unary_op(ZEND_BOOL_NOT, &A, &B TSRMLS_CC); }
expr_wo_var(A) ::= BW_NOT expr(B).                       { zend_do_unary_op(ZEND_BW_NOT, &A, &B TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_IDENTICAL expr(C).         { zend_do_binary_op(ZEND_IS_IDENTICAL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_NOT_IDENTICAL expr(C).     { zend_do_binary_op(ZEND_IS_NOT_IDENTICAL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_EQUAL expr(C).             { zend_do_binary_op(ZEND_IS_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_NOT_EQUAL expr(C).         { zend_do_binary_op(ZEND_IS_NOT_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_SMALLER expr(C).           { zend_do_binary_op(ZEND_IS_SMALLER, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_SMALLER_OR_EQUAL expr(C).  { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &A, &B, &C TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_GREATER expr(C).           { zend_do_binary_op(ZEND_IS_SMALLER, &A, &C, &B TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) IS_GREATER_OR_EQUAL expr(C).  { zend_do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &A, &C, &B TSRMLS_CC); }
expr_wo_var(A) ::= expr(B) INSTANCEOF class_name_reference(C). { zend_do_instanceof(&A, &B, &C, 0 TSRMLS_CC); }
expr_wo_var(A) ::= LPAREN expr(B) RPAREN.                { A = B; }
expr_wo_var(A) ::= ternary(B).                           { A = B; }
expr_wo_var(A) ::= short_ternary(B).                     { A = B; }
expr_wo_var(A) ::= internal_functions_in_yacc(B).        { A = B; }
expr_wo_var(A) ::= INT_CAST expr(B).                     { zend_do_cast(&A, &B, IS_LONG TSRMLS_CC); }
expr_wo_var(A) ::= DOUBLE_CAST expr(B).                  { zend_do_cast(&A, &B, IS_DOUBLE TSRMLS_CC); }
expr_wo_var(A) ::= STRING_CAST expr(B).                  { zend_do_cast(&A, &B, IS_STRING TSRMLS_CC); }
expr_wo_var(A) ::= ARRAY_CAST expr(B).                   { zend_do_cast(&A, &B, IS_ARRAY TSRMLS_CC); }
expr_wo_var(A) ::= OBJECT_CAST expr(B).                  { zend_do_cast(&A, &B, IS_OBJECT TSRMLS_CC); }
expr_wo_var(A) ::= BOOL_CAST expr(B).                    { zend_do_cast(&A, &B, IS_BOOL TSRMLS_CC); }
expr_wo_var(A) ::= SCALAR_CAST expr(B).                  { zend_do_cast(&A, &B, IS_SCALAR TSRMLS_CC); }
expr_wo_var(A) ::= NUMERIC_CAST expr(B).                 { zend_do_cast(&A, &B, IS_NUMERIC TSRMLS_CC); }
expr_wo_var(A) ::= UNSET_CAST expr(B).                   { zend_do_cast(&A, &B, IS_NULL TSRMLS_CC); }
expr_wo_var(A) ::= EXIT exit_expr(B).                    { zend_do_exit(&A, &B TSRMLS_CC); }
expr_wo_var(A) ::= silent(B) expr(C). [IF]               { zend_do_end_silence(&B TSRMLS_CC); A = C; }
expr_wo_var(A) ::= ARRAY LPAREN array_pairs(B) RPAREN.   { A = B; }
expr_wo_var(A) ::= scalar(B).                            { A = B; }
expr_wo_var(A) ::= BACKQUOTE backticks_expr(B) BACKQUOTE.{ zend_do_shell_exec(&A, &B TSRMLS_CC); }
expr_wo_var(A) ::= PRINT expr(B).                        { zend_do_print(&A, &B TSRMLS_CC); }
expr_wo_var(A) ::= closure(B).                           { A = B; }
expr_wo_var(A) ::= PLUS(B) expr(C). [INC]                { ZVAL_LONG(&B.u.constant, 0);
                                                           if (C.op_type == IS_CONST) {
                                                               add_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); 
                                                               A = C;
                                                           } else {
                                                               B.op_type = IS_CONST;
                                                               INIT_PZVAL(&B.u.constant);
                                                               zend_do_binary_op(ZEND_ADD, &A, &B, &C TSRMLS_CC); 
                                                           }
                                                          }
expr_wo_var(A) ::= MINUS(B) expr(C). [INC]               { ZVAL_LONG(&B.u.constant, 0);
                                                           if (C.op_type == IS_CONST) {
                                                               sub_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC);
                                                               A = C;
                                                           } else {
                                                               B.op_type = IS_CONST;
                                                               INIT_PZVAL(&B.u.constant);
                                                               zend_do_binary_op(ZEND_SUB, &A, &B, &C TSRMLS_CC);
                                                           }
                                                         }

closure(A) ::= closure_i(B) parameters RPAREN lexical_vars LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B[1] TSRMLS_CC); A = B[0]; }
closure(A) ::= closure_ii(B) parameters RPAREN lexical_vars LBRACE inner_statement_list RBRACE. { zend_do_end_function_declaration(&B[1] TSRMLS_CC); A = B[0]; }

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

function(A) ::= FUNCTION. { A.u.op.opline_num = CG(zend_lineno); }


lexical_vars ::= .
lexical_vars ::= USE LPAREN lexical_var_list RPAREN.

lexical_var_list ::= lexical_var_list COMMA VARIABLE(B).        { zend_do_fetch_lexical_variable(&B, 0 TSRMLS_CC); }
lexical_var_list ::= lexical_var_list COMMA BW_AND VARIABLE(B). { zend_do_fetch_lexical_variable(&B, 1 TSRMLS_CC); }
lexical_var_list ::= VARIABLE(B).                               { zend_do_fetch_lexical_variable(&B, 0 TSRMLS_CC); }
lexical_var_list ::= BW_AND VARIABLE(B).                        { zend_do_fetch_lexical_variable(&B, 1 TSRMLS_CC); }


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

function_call(A) ::= function_call_ns_i(B) fcall_parameters(C) RPAREN. {
    zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
    zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_ns_ii(B)	fcall_parameters(C) RPAREN. {
    zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
    zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_ns_iii(B) fcall_parameters(C) RPAREN. {
    zend_do_end_function_call(&B[0], &A, &C, 0, B[1].u.op.opline_num TSRMLS_CC);
    zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_cls_i(B) fcall_parameters(C) RPAREN. {
    zend_do_end_function_call(B[1].u.op.opline_num?NULL:&B[0], &A, &C, B[1].u.op.opline_num, B[1].u.op.opline_num TSRMLS_CC);
    zend_do_extended_fcall_end(TSRMLS_C);
}

function_call(A) ::= function_call_cls_ii fcall_parameters(C) RPAREN. { zend_do_end_function_call(NULL, &A, &C, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_i fcall_parameters(B) RPAREN. { zend_do_end_function_call(NULL, &A, &B, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_ii fcall_parameters(B) RPAREN. { zend_do_end_function_call(NULL, &A, &B, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

function_call(A) ::= function_call_var_iii(B) fcall_parameters(C) RPAREN. { zend_do_end_function_call(&B, &A, &C, 0, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}

class_name(A) ::= STATIC.                                   { A.op_type = IS_CONST; ZVAL_STRINGL(&A.u.constant, "static", sizeof("static")-1, 1); }
class_name(A) ::= namespace_name(B).                        { A = B; }
class_name(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); }
class_name(A) ::= NS_SEPARATOR namespace_name(B).           { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                              memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                              tmp[0] = '\\';
                                                              efree(Z_STRVAL(B.u.constant));
                                                              Z_STRVAL(B.u.constant) = tmp;
                                                              ++Z_STRLEN(B.u.constant);
                                                              A = B; }

fully_qualified_class_name(A) ::= namespace_name(B).                        { A = B; }
fully_qualified_class_name(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B). { A.op_type = IS_CONST; ZVAL_EMPTY_STRING(&A.u.constant);  zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC); }
fully_qualified_class_name(A) ::= NS_SEPARATOR namespace_name(B).           { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                              memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                              tmp[0] = '\\';
                                                                              efree(Z_STRVAL(B.u.constant));
                                                                              Z_STRVAL(B.u.constant) = tmp;
                                                                              ++Z_STRLEN(B.u.constant); A = B; }

class_name_reference(A) ::= class_name(B).                   { zend_do_fetch_class(&A, &B TSRMLS_CC); }
class_name_reference(A) ::= dynamic_class_name_reference(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); zend_do_fetch_class(&A, &B TSRMLS_CC); }


dynamic_class_name_reference_i  ::= base_variable(B) OBJECT_OPERATOR.                  { zend_do_push_object(&B TSRMLS_CC); }
dynamic_class_name_reference_ii ::= dynamic_class_name_reference_i object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

dynamic_class_name_reference(A) ::= dynamic_class_name_reference_ii dynamic_class_name_variable_properties. {
    zend_do_pop_object(&A TSRMLS_CC);
    A.EA = ZEND_PARSED_MEMBER;
}

dynamic_class_name_reference(A) ::= base_variable(B). { A = B; }

dynamic_class_name_variable_properties ::= .
dynamic_class_name_variable_properties ::= dynamic_class_name_variable_properties dynamic_class_name_variable_property.

dynamic_class_name_variable_property ::= OBJECT_OPERATOR object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

exit_expr    ::= .
exit_expr(A) ::= LPAREN RPAREN.		{ memset(&A, 0, sizeof(znode)); A.op_type = IS_UNUSED; }
exit_expr(A) ::= LPAREN expr(B) RPAREN.	{ A = B; }

backticks_expr(A) ::= .                           { ZVAL_EMPTY_STRING(&A.u.constant); INIT_PZVAL(&A.u.constant); A.op_type = IS_CONST; }
backticks_expr(A) ::= ENCAPSED_AND_WHITESPACE(B). { A = B; }
backticks_expr(A) ::= encaps_list(B).             { A = B; }

ctor_arguments(A) ::= .                                  { Z_LVAL(A.u.constant) = 0; }
ctor_arguments(A) ::= LPAREN fcall_parameters(B) RPAREN. { A = B; }

common_scalar(A) ::= LNUMBER(B).                    { A = B; }
common_scalar(A) ::= DNUMBER(B).                    { A = B; }
common_scalar(A) ::= CONSTANT_ENCAPSED_STRING(B).   { A = B; }
common_scalar(A) ::= LINE(B).                       { A = B; }
common_scalar(A) ::= FILE(B).                       { A = B; }
common_scalar(A) ::= DIR(B).                        { A = B; }
common_scalar(A) ::= CLASS_C(B).                    { A = B; }
common_scalar(A) ::= METHOD_C(B).                   { A = B; }
common_scalar(A) ::= FUNC_C(B).                     { A = B; }
common_scalar(A) ::= NS_C(B).                       { A = B; }
common_scalar(A) ::= START_HEREDOC(B) ENCAPSED_AND_WHITESPACE(C) END_HEREDOC. { A = C;
                                                                                CG(heredoc) = Z_STRVAL(B.u.constant);
                                                                                CG(heredoc_len) = Z_STRLEN(B.u.constant); 
                                                                              }
common_scalar(A) ::= START_HEREDOC(B) END_HEREDOC.                            { ZVAL_EMPTY_STRING(&A.u.constant);
                                                                                INIT_PZVAL(&A.u.constant);
                                                                                A.op_type = IS_CONST;
                                                                                CG(heredoc) = Z_STRVAL(B.u.constant);
                                                                                CG(heredoc_len) = Z_STRLEN(B.u.constant);
                                                                              }

static_scalar(A) ::= common_scalar(B).                              { A = B; }
static_scalar(A) ::= namespace_name(B).                             { zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 1 TSRMLS_CC); }
static_scalar(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B).      { A.op_type = IS_CONST;
                                                                      ZVAL_EMPTY_STRING(&A.u.constant);
                                                                      zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC);
                                                                      B = A;
                                                                      zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 0 TSRMLS_CC);
                                                                    }
static_scalar(A) ::= NS_SEPARATOR namespace_name(B).                { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                      memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                      tmp[0] = '\\';
                                                                      efree(Z_STRVAL(B.u.constant));
                                                                      Z_STRVAL(B.u.constant) = tmp;
                                                                      ++Z_STRLEN(B.u.constant);
                                                                      zend_do_fetch_constant(&A, NULL, &B, ZEND_CT, 0 TSRMLS_CC);
                                                                    }
static_scalar(A) ::= PLUS(B) static_scalar(C).                      { ZVAL_LONG(&B.u.constant, 0);
                                                                      add_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC);
                                                                      A = C;
                                                                    }
static_scalar(A) ::= MINUS(B) static_scalar(C).                     { ZVAL_LONG(&B.u.constant, 0);
                                                                      sub_function(&C.u.constant, &B.u.constant, &C.u.constant TSRMLS_CC); 
                                                                      A = C; }
static_scalar(A) ::= ARRAY LPAREN static_array_pair_list(B) RPAREN. { A = B; Z_TYPE(A.u.constant) = IS_CONSTANT_ARRAY; }
static_scalar(A) ::= static_class_constant(B).                      { A = B; }

static_class_constant(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C). { zend_do_fetch_constant(&A, &B, &C, ZEND_CT, 0 TSRMLS_CC); }

scalar(A) ::= STRING_VARNAME(B).                                    { A = B; }
scalar(A) ::= class_constant(B).                                    { A = B; }
scalar(A) ::= namespace_name(B).                                    { zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 1 TSRMLS_CC); }
scalar(A) ::= NAMESPACE NS_SEPARATOR namespace_name(B).             { A.op_type = IS_CONST;
                                                                      ZVAL_EMPTY_STRING(&A.u.constant);
                                                                      zend_do_build_namespace_name(&A, &A, &B TSRMLS_CC);
                                                                      B = A;
                                                                      zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 0 TSRMLS_CC);
                                                                    }
scalar(A) ::= NS_SEPARATOR namespace_name(B).                       { char *tmp = estrndup(Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                      memcpy(&(tmp[1]), Z_STRVAL(B.u.constant), Z_STRLEN(B.u.constant)+1);
                                                                      tmp[0] = '\\';
                                                                      efree(Z_STRVAL(B.u.constant));
                                                                      Z_STRVAL(B.u.constant) = tmp;
                                                                      ++Z_STRLEN(B.u.constant);
                                                                      zend_do_fetch_constant(&A, NULL, &B, ZEND_RT, 0 TSRMLS_CC);
                                                                    }
scalar(A) ::= common_scalar(B).                                     { A = B; }
scalar(A) ::= QUOTE encaps_list(B) QUOTE.                           { A = B; }
scalar(A) ::= START_HEREDOC(B) encaps_list(C) END_HEREDOC.          { A = C;
                                                                      CG(heredoc) = Z_STRVAL(B.u.constant);
                                                                      CG(heredoc_len) = Z_STRLEN(B.u.constant);
                                                                    }

static_array_pair_list(A) ::= .                                                   { A.op_type = IS_CONST;
                                                                                    INIT_PZVAL(&A.u.constant);
                                                                                    array_init(&A.u.constant); }
static_array_pair_list(A) ::= non_empty_static_array_pair_list(B) possible_comma. { A = B; }

possible_comma ::= .
possible_comma ::= COMMA.

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


expr(A) ::= r_variable(B).  { A = B; }
expr(A) ::= expr_wo_var(B). { A = B; }

r_variable(A)  ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); A = B; }
w_variable(A)  ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_W, 0 TSRMLS_CC); A = B; zend_check_writable_variable(&B); }
rw_variable(A) ::= variable(B). { zend_do_end_variable_parse(&B, BP_VAR_RW, 0 TSRMLS_CC); A = B; zend_check_writable_variable(&B); }

variable_i(A) ::= base_variable_with_function_calls(B) OBJECT_OPERATOR. { zend_do_push_object(&B TSRMLS_CC); A = B; }
variable_ii ::= object_property(B).                                     { zend_do_push_object(&B TSRMLS_CC); }

variable(A) ::= variable_i(B) variable_ii method_or_not(C) variable_properties(D). { zend_do_pop_object(&A TSRMLS_CC); A.EA = B.EA | (D.EA ? D.EA : C.EA); }
variable(A) ::= base_variable_with_function_calls(B). { A = B; }

variable_properties(A) ::= .                                         { A.EA = 0; }
variable_properties(A) ::= variable_properties variable_property(B). { A.EA = B.EA; }

variable_property_i ::= OBJECT_OPERATOR object_property(B). { zend_do_push_object(&B TSRMLS_CC); }

variable_property(A) ::= variable_property_i method_or_not(B). { A.EA = B.EA; }

array_method_dereference(A) ::= array_method_dereference(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
array_method_dereference(A) ::= method(B) LBRACKET dim_offset(C) RBRACKET.                   { fetch_array_dim(&A, &B, &C TSRMLS_CC); }

method_i(A) ::= LPAREN(B).                              { zend_do_pop_object(&B TSRMLS_CC); zend_do_begin_method_call(&B TSRMLS_CC); A = B; }
method(A)   ::= method_i(B) fcall_parameters(C) RPAREN. { zend_do_end_function_call(&B, &A, &C, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }

method_or_not(A) ::= .                            { A.EA = ZEND_PARSED_MEMBER; }
method_or_not(A) ::= method(B).                   { A = B; zend_do_push_object(&A TSRMLS_CC); A.EA = ZEND_PARSED_METHOD_CALL; }
method_or_not(A) ::= array_method_dereference(B). { A = B; zend_do_push_object(&A TSRMLS_CC); }

variable_without_objects(A) ::= reference_variable(B).                              { A = B; }
variable_without_objects(A) ::= simple_indirect_reference(B) reference_variable(C). { zend_do_indirect_references(&A, &B, &C TSRMLS_CC); }

static_member(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C).          { A = C; zend_do_fetch_static_member(&A, &B TSRMLS_CC); }
static_member(A) ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM variable_without_objects(C). { A = C; zend_do_fetch_static_member(&A, &B TSRMLS_CC); }

variable_class_name(A) ::= reference_variable(B). { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); A = B; }

array_function_dereference_i(A) ::= function_call(B). { zend_do_begin_variable_parse(TSRMLS_C); A = B; A.EA = ZEND_PARSED_FUNCTION_CALL; } 

array_function_dereference(A) ::= array_function_dereference(B) LBRACKET dim_offset(C) RBRACKET.   { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
array_function_dereference(A) ::= array_function_dereference_i(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }

base_variable_with_function_calls(A) ::= base_variable(B).              { A = B; }
base_variable_with_function_calls(A) ::= array_function_dereference(B). { A = B; }
base_variable_with_function_calls(A) ::= function_call(B).              { zend_do_begin_variable_parse(TSRMLS_C); A = B; A.EA = ZEND_PARSED_FUNCTION_CALL; }

base_variable(A) ::= reference_variable(B).                              { A = B; A.EA = ZEND_PARSED_VARIABLE; }
base_variable(A) ::= simple_indirect_reference(B) reference_variable(C). { zend_do_indirect_references(&A, &B, &C TSRMLS_CC); A.EA = ZEND_PARSED_VARIABLE; }
base_variable(A) ::= static_member(B).                                   { A = B; A.EA = ZEND_PARSED_STATIC_MEMBER; }

reference_variable(A) ::= reference_variable(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
reference_variable(A) ::= reference_variable(B) LBRACE expr(C) RBRACE.           { fetch_string_offset(&A, &B, &C TSRMLS_CC); }
reference_variable(A) ::= compound_variable(B).                                  { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }

compound_variable(A) ::= VARIABLE(B).                   { A = B; }
compound_variable(A) ::= DOLLAR LBRACE expr(B) RBRACE.  { A = B; }

dim_offset(A) ::= .         { A.op_type = IS_UNUSED; }
dim_offset(A) ::= expr(B).  { A = B; }

object_property(A) ::= object_dim_list(B).          { A = B; }
object_property(A) ::= variable_without_objects(B). { znode tmp_znode;
                                                      zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC);
                                                      zend_do_pop_object(&tmp_znode TSRMLS_CC);
                                                      zend_do_fetch_property(&A, &tmp_znode, &B TSRMLS_CC);
                                                    }


object_dim_list(A) ::= object_dim_list(B) LBRACKET dim_offset(C) RBRACKET. { fetch_array_dim(&A, &B, &C TSRMLS_CC); }
object_dim_list(A) ::= object_dim_list(B) LBRACE expr(C) RBRACE. { fetch_string_offset(&A, &B, &C TSRMLS_CC); }
object_dim_list(A) ::= variable_name(B). { znode tmp_znode;
                                           zend_do_pop_object(&tmp_znode TSRMLS_CC);
                                           zend_do_fetch_property(&A, &tmp_znode, &B TSRMLS_CC);
                                         }
variable_name(A) ::= STRING(B).             { A = B; }
variable_name(A) ::= LBRACE expr(B) RBRACE. { A = B; }

simple_indirect_reference(A) ::= DOLLAR.                              { Z_LVAL(A.u.constant) = 1; }
simple_indirect_reference(A) ::= simple_indirect_reference(B) DOLLAR. { Z_LVAL(B.u.constant)++; A = B; }

assignment_list ::= assignment_list COMMA assignment_list_element.
assignment_list ::= assignment_list_element.

assignment_list_element_i ::= LIST LPAREN. { zend_do_new_list_begin(TSRMLS_C); }

assignment_list_element ::= variable(B).                                      { zend_do_add_list_element(&B TSRMLS_CC); }
assignment_list_element ::= assignment_list_element_i assignment_list RPAREN. { zend_do_new_list_end(TSRMLS_C); }
assignment_list_element ::= .                                                 { zend_do_add_list_element(NULL TSRMLS_CC); }

array_pairs(A) ::= .                                            { zend_do_init_array(&A, NULL, NULL, 0 TSRMLS_CC); }
array_pairs(A) ::= array_pairs_list(B) possible_comma. { A = B; }

array_pairs_list(A) ::= array_pairs_list(B) COMMA expr(C) DOUBLE_ARROW expr(D).              { zend_do_add_array_element(&B, &D, &C, 0 TSRMLS_CC); A = B; }
array_pairs_list(A) ::= array_pairs_list(B) COMMA expr(C).                                   { zend_do_add_array_element(&B, &C, NULL, 0 TSRMLS_CC); A = B; }
array_pairs_list(A) ::= array_pairs_list(B) COMMA expr(C) DOUBLE_ARROW BW_AND w_variable(D). { zend_do_add_array_element(&B, &D, &C, 1 TSRMLS_CC); A = B; }
array_pairs_list(A) ::= array_pairs_list(B) COMMA BW_AND w_variable(C).                      { zend_do_add_array_element(&B, &C, NULL, 1 TSRMLS_CC); A = B; }
array_pairs_list(A) ::= expr(B) DOUBLE_ARROW expr(C).                                        { zend_do_init_array(&A, &C, &B, 0 TSRMLS_CC); }
array_pairs_list(A) ::= expr(B).                                                             { zend_do_init_array(&A, &B, NULL, 0 TSRMLS_CC); }
array_pairs_list(A) ::= expr(B) DOUBLE_ARROW BW_AND w_variable(C).                           { zend_do_init_array(&A, &C, &B, 1 TSRMLS_CC); }
array_pairs_list(A) ::= BW_AND w_variable(B).                                                { zend_do_init_array(&A, &B, NULL, 1 TSRMLS_CC); }

encaps_list(A) ::= encaps_list(B) encaps_var(C).              { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&A, &B, &C TSRMLS_CC); }
encaps_list(A) ::= encaps_list(B) ENCAPSED_AND_WHITESPACE(C). { zend_do_add_string(&B, &B, &C TSRMLS_CC); A = B; }
encaps_list(A) ::= encaps_var(B).                             { zend_do_end_variable_parse(&B, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&A, NULL, &B TSRMLS_CC); }
encaps_list(A) ::= ENCAPSED_AND_WHITESPACE(B) encaps_var(C).  { zend_do_add_string(&A, NULL, &B TSRMLS_CC); zend_do_end_variable_parse(&C, BP_VAR_R, 0 TSRMLS_CC); zend_do_add_variable(&A, &A, &C TSRMLS_CC); }

encaps_var_i(A) ::= VARIABLE(B) LBRACKET. { zend_do_begin_variable_parse(TSRMLS_C); A = B; }

encaps_var(A) ::= VARIABLE(B).                                   { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }
encaps_var(A) ::= encaps_var_i(B) encaps_var_offset(C) RBRACKET. { fetch_array_begin(&A, &B, &C TSRMLS_CC); }
encaps_var(A) ::= VARIABLE(B) OBJECT_OPERATOR(C) STRING(D).      { zend_do_begin_variable_parse(TSRMLS_C); fetch_simple_variable(&C, &B, 1 TSRMLS_CC); zend_do_fetch_property(&A, &C, &D TSRMLS_CC); }
encaps_var(A) ::= DOLLAR_OPEN_CURLY_BRACES expr(B) RBRACE.       { zend_do_begin_variable_parse(TSRMLS_C);  fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }
encaps_var(A) ::= DOLLAR_OPEN_CURLY_BRACES STRING_VARNAME(B) LBRACKET expr(C) RBRACKET RBRACE. { zend_do_begin_variable_parse(TSRMLS_C);  fetch_array_begin(&A, &B, &C TSRMLS_CC); }
encaps_var(A) ::= CURLY_OPEN variable(B) RBRACE.                 { A = B; }

encaps_var_offset(A) ::= STRING(B).     { A = B; }
encaps_var_offset(A) ::= NUM_STRING(B). { A = B; }
encaps_var_offset(A) ::= VARIABLE(B).   { fetch_simple_variable(&A, &B, 1 TSRMLS_CC); }

internal_functions_in_yacc(A) ::= ISSET LPAREN isset_variables(B) RPAREN. { A = B; }
internal_functions_in_yacc(A) ::= EMPTY LPAREN variable(B) RPAREN.        { zend_do_isset_or_isempty(ZEND_ISEMPTY, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= INCLUDE expr(B).                        { zend_do_include_or_eval(ZEND_INCLUDE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= INCLUDE_ONCE expr(B).                   { zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= EVAL LPAREN expr(B) RPAREN.             { zend_do_include_or_eval(ZEND_EVAL, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= REQUIRE expr(B).                        { zend_do_include_or_eval(ZEND_REQUIRE, &A, &B TSRMLS_CC); }
internal_functions_in_yacc(A) ::= REQUIRE_ONCE expr(B).                   { zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &A, &B TSRMLS_CC); }

isset_variables_i(A) ::= isset_variables(B) COMMA(C). { zend_do_boolean_and_begin(&B, &C TSRMLS_CC); A[0] = B; A[1] = C; }

isset_variables(A) ::= variable(B).                      { zend_do_isset_or_isempty(ZEND_ISSET, &A, &B TSRMLS_CC); }
isset_variables(A) ::= isset_variables_i(B) variable(C). { znode tmp;
                                                           zend_do_isset_or_isempty(ZEND_ISSET, &tmp, &C TSRMLS_CC);
                                                           zend_do_boolean_and_end(&A, &B[0], &tmp, &B[1] TSRMLS_CC);
                                                         }

class_constant(A) ::= class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C).          { zend_do_fetch_constant(&A, &B, &C, ZEND_RT, 0 TSRMLS_CC); }
class_constant(A) ::= variable_class_name(B) PAAMAYIM_NEKUDOTAYIM STRING(C). { zend_do_fetch_constant(&A, &B, &C, ZEND_RT, 0 TSRMLS_CC); }


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
