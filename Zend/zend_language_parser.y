%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2002 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* 
 * LALR shift/reduce conflicts and how they are resolved:
 *
 * - 2 shift/reduce conflicts due to the dangeling elseif/else ambiguity.  Solved by shift.
 * - 1 shift/reduce conflict due to arrays within encapsulated strings. Solved by shift. 
 * - 1 shift/reduce conflict due to objects within encapsulated strings.  Solved by shift.
 * 
 */


#include "zend_compile.h"
#include "zend.h"
#include "zend_list.h"
#include "zend_globals.h"
#include "zend_API.h"

#define YYERROR_VERBOSE
#define YYSTYPE znode
#ifdef ZTS
# define YYPARSE_PARAM tsrm_ls
# define YYLEX_PARAM tsrm_ls
#endif


%}

%pure_parser
%expect 4

%left T_INCLUDE T_INCLUDE_ONCE T_EVAL T_REQUIRE T_REQUIRE_ONCE
%left ','
%left T_LOGICAL_OR
%left T_LOGICAL_XOR
%left T_LOGICAL_AND
%right T_PRINT
%left '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL
%left '?' ':'
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
%right '!' '~' T_INC T_DEC T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right '['
%nonassoc T_NEW
%token T_EXIT
%token T_IF
%left T_ELSEIF
%left T_ELSE
%left T_ENDIF
%token T_LNUMBER
%token T_DNUMBER
%token T_STRING
%token T_STRING_VARNAME
%token T_VARIABLE
%token T_NUM_STRING
%token T_INLINE_HTML
%token T_CHARACTER
%token T_BAD_CHARACTER
%token T_ENCAPSED_AND_WHITESPACE
%token T_CONSTANT_ENCAPSED_STRING
%token T_ECHO
%token T_DO
%token T_WHILE
%token T_ENDWHILE
%token T_FOR
%token T_ENDFOR
%token T_FOREACH
%token T_ENDFOREACH
%token T_DECLARE
%token T_ENDDECLARE
%token T_AS
%token T_SWITCH
%token T_ENDSWITCH
%token T_CASE
%token T_DEFAULT
%token T_BREAK
%token T_CONTINUE
%token T_OLD_FUNCTION
%token T_FUNCTION
%token T_CONST
%token T_RETURN
%token T_TRY
%token T_CATCH
%token T_THROW
%token T_USE
%token T_GLOBAL
%token T_STATIC
%token T_PRIVATE
%token T_PROTECTED
%token T_VAR
%token T_UNSET
%token T_ISSET
%token T_EMPTY
%token T_CLASS
%token T_EXTENDS
%token T_OBJECT_OPERATOR
%token T_DOUBLE_ARROW
%token T_LIST
%token T_ARRAY
%token T_CLASS_C
%token T_FUNC_C
%token T_LINE
%token T_FILE
%token T_COMMENT
%token T_ML_COMMENT
%token T_OPEN_TAG
%token T_OPEN_TAG_WITH_ECHO
%token T_CLOSE_TAG
%token T_WHITESPACE
%token T_START_HEREDOC
%token T_END_HEREDOC
%token T_DOLLAR_OPEN_CURLY_BRACES
%token T_CURLY_OPEN
%token T_PAAMAYIM_NEKUDOTAYIM
%token T_IMPORT T_FROM

%% /* Rules */

start:
	top_statement_list
;

top_statement_list:
		top_statement_list  { zend_do_extended_info(TSRMLS_C); } top_statement { HANDLE_INTERACTIVE(); }
	|	/* empty */
;


top_statement:
		statement
	|	declaration_statement	{ zend_do_early_binding(TSRMLS_C); }
;


inner_statement_list:
		inner_statement_list  { zend_do_extended_info(TSRMLS_C); } inner_statement { HANDLE_INTERACTIVE(); }
	|	/* empty */
;


inner_statement:
		statement
	|	declaration_statement
;


statement:
		unticked_statement { zend_do_ticks(TSRMLS_C); }
;

unticked_statement:
		'{' inner_statement_list '}'
	|	T_IF '(' expr ')' { zend_do_if_cond(&$3, &$4 TSRMLS_CC); } statement { zend_do_if_after_statement(&$4, 1 TSRMLS_CC); } elseif_list else_single { zend_do_if_end(TSRMLS_C); }
	|	T_IF '(' expr ')' ':' { zend_do_if_cond(&$3, &$4 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$4, 1 TSRMLS_CC); } new_elseif_list new_else_single T_ENDIF ';' { zend_do_if_end(TSRMLS_C); }
	|	T_WHILE '(' { $1.u.opline_num = get_next_op_number(CG(active_op_array));  } expr  ')' { zend_do_while_cond(&$4, &$5 TSRMLS_CC); } while_statement { zend_do_while_end(&$1, &$5 TSRMLS_CC); }
	|	T_DO { $1.u.opline_num = get_next_op_number(CG(active_op_array));  zend_do_do_while_begin(TSRMLS_C); } statement T_WHILE '(' { $5.u.opline_num = get_next_op_number(CG(active_op_array)); } expr ')' ';' { zend_do_do_while_end(&$1, &$5, &$7 TSRMLS_CC); }
	|	T_FOR 
			'('
				for_expr
			';' { zend_do_free(&$3 TSRMLS_CC); $4.u.opline_num = get_next_op_number(CG(active_op_array)); }
				for_expr
			';' { zend_do_extended_info(TSRMLS_C); zend_do_for_cond(&$6, &$7 TSRMLS_CC); }
				for_expr
			')' { zend_do_free(&$9 TSRMLS_CC); zend_do_for_before_statement(&$4, &$7 TSRMLS_CC); }
			for_statement { zend_do_for_end(&$7 TSRMLS_CC); }
	|	T_SWITCH '(' expr ')'	{ zend_do_switch_cond(&$3 TSRMLS_CC); } switch_case_list { zend_do_switch_end(&$6 TSRMLS_CC); }
	|	T_BREAK ';'				{ zend_do_brk_cont(ZEND_BRK, NULL TSRMLS_CC); }
	|	T_BREAK expr ';'		{ zend_do_brk_cont(ZEND_BRK, &$2 TSRMLS_CC); }
	|	T_CONTINUE ';'			{ zend_do_brk_cont(ZEND_CONT, NULL TSRMLS_CC); }
	|	T_CONTINUE expr ';'		{ zend_do_brk_cont(ZEND_CONT, &$2 TSRMLS_CC); }
	|	T_RETURN ';'						{ zend_do_return(NULL, 0 TSRMLS_CC); }
	|	T_RETURN expr_without_variable ';'	{ zend_do_return(&$2, 0 TSRMLS_CC); }
	|	T_RETURN variable ';'				{ zend_do_return(&$2, 1 TSRMLS_CC); }
	|	T_GLOBAL global_var_list ';'
	|	T_STATIC static_var_list ';'
	|	T_ECHO echo_expr_list ';'
	|	T_INLINE_HTML			{ zend_do_echo(&$1 TSRMLS_CC); }
	|	expr ';'				{ zend_do_free(&$1 TSRMLS_CC); }
	|	T_USE use_filename ';'		{ zend_error(E_COMPILE_ERROR,"use: Not yet supported. Please use include_once() or require_once()");  zval_dtor(&$2.u.constant); }
	|	T_UNSET '(' unset_variables ')' ';'
	|	T_FOREACH '(' variable T_AS { zend_do_foreach_begin(&$1, &$3, &$2, &$4, 1 TSRMLS_CC); } w_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$6, &$7, &$4 TSRMLS_CC); } foreach_statement { zend_do_foreach_end(&$1, &$2 TSRMLS_CC); }
	|	T_FOREACH '(' expr_without_variable T_AS { zend_do_foreach_begin(&$1, &$3, &$2, &$4, 0 TSRMLS_CC); } w_variable foreach_optional_arg ')' { zend_do_foreach_cont(&$6, &$7, &$4 TSRMLS_CC); } foreach_statement { zend_do_foreach_end(&$1, &$2 TSRMLS_CC); }
	|	T_DECLARE { zend_do_declare_begin(TSRMLS_C); } '(' declare_list ')' declare_statement { zend_do_declare_end(TSRMLS_C); }
	|	';'		/* empty statement */
	|	T_TRY { zend_do_try(&$1 TSRMLS_CC); } '{' inner_statement_list '}'
		T_CATCH '(' catch_or_import_class_entry T_VARIABLE ')' { zend_do_begin_catch(&$1, &$8, &$9, 1 TSRMLS_CC); } '{' inner_statement_list '}' { zend_do_end_catch(&$1 TSRMLS_CC); }
		additional_catches
	|	T_THROW expr ';' { zend_do_throw(&$2 TSRMLS_CC); }
	|	T_IMPORT { zend_do_begin_import(TSRMLS_C); } import_rule T_FROM catch_or_import_class_entry { zend_do_end_import(&$5 TSRMLS_CC); } ';'
;


import_rule:
		'*'	{ zend_do_import(T_FUNCTION, NULL TSRMLS_CC); zend_do_import(T_CLASS, NULL TSRMLS_CC); zend_do_import(T_CONST, NULL TSRMLS_CC); }
	|	import_commands
;

import_commands:
		import_commands ',' import_command
	|	import_command
;

import_command:
		T_FUNCTION T_STRING { zend_do_import(T_FUNCTION, &$2 TSRMLS_CC); }
	|	T_CLASS	T_STRING	{ zend_do_import(T_CLASS, &$2 TSRMLS_CC); }
	|	T_CONST	T_STRING	{ zend_do_import(T_CONST, &$2 TSRMLS_CC); }
	|	T_FUNCTION '*'		{ zend_do_import(T_FUNCTION, NULL TSRMLS_CC); }
	|	T_CLASS	'*'			{ zend_do_import(T_CLASS, NULL TSRMLS_CC); }
	|	T_CONST	'*'			{ zend_do_import(T_CONST, NULL TSRMLS_CC); }
;

additional_catches:
		non_empty_additional_catches
	|	/* empty */
;

non_empty_additional_catches:
		non_empty_additional_catches T_CATCH '(' catch_or_import_class_entry T_VARIABLE ')' { zend_do_begin_catch(&$2, &$4, &$5, 0 TSRMLS_CC); } '{' inner_statement_list '}' { zend_do_end_catch(&$2 TSRMLS_CC); }
	|	T_CATCH '(' catch_or_import_class_entry T_VARIABLE ')' { zend_do_begin_catch(&$1, &$3, &$4, 0 TSRMLS_CC); } '{' inner_statement_list '}' { zend_do_end_catch(&$1 TSRMLS_CC); }
;


unset_variables:
		unset_variable
	|	unset_variables ',' unset_variable
;

unset_variable:
		variable	{ zend_do_end_variable_parse(BP_VAR_UNSET, 0 TSRMLS_CC); zend_do_unset(&$1 TSRMLS_CC); }
;

use_filename:
		T_CONSTANT_ENCAPSED_STRING			{ $$ = $1; }
	|	'(' T_CONSTANT_ENCAPSED_STRING ')'	{ $$ = $2; }
;


declaration_statement:
		unticked_declaration_statement	{ zend_do_ticks(TSRMLS_C); }
;


unticked_declaration_statement:
		T_FUNCTION { $1.u.opline_num = CG(zend_lineno); } is_reference T_STRING { zend_do_begin_function_declaration(&$1, &$4, 0, $3.op_type TSRMLS_CC); }
			'(' parameter_list ')' '{' inner_statement_list '}' { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
	|	T_OLD_FUNCTION { $1.u.opline_num = CG(zend_lineno); } is_reference T_STRING  { zend_do_begin_function_declaration(&$1, &$4, 0, $3.op_type TSRMLS_CC); }
			parameter_list '(' inner_statement_list ')' ';' { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
	|	T_CLASS declaration_class_name { zend_do_begin_class_declaration(&$1, &$2, NULL TSRMLS_CC); } '{' class_statement_list '}' { zend_do_end_class_declaration(&$1 TSRMLS_CC); }
	|	T_CLASS declaration_class_name T_EXTENDS T_STRING { zend_do_begin_class_declaration(&$1, &$2, &$4 TSRMLS_CC); } '{' class_statement_list '}' { zend_do_end_class_declaration(&$1 TSRMLS_CC); }
;

declaration_class_name:
	|	parse_class_name_entry T_STRING { do_fetch_class_name(&$$, &$1, &$2, 0 TSRMLS_CC); }
	|	T_STRING { $$ = $1; zend_str_tolower($$.u.constant.value.str.val, $$.u.constant.value.str.len); }
;

foreach_optional_arg:
		/* empty */				{ $$.op_type = IS_UNUSED; }
	|	T_DOUBLE_ARROW w_variable	{ $$ = $2; }
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
	|	elseif_list T_ELSEIF '(' expr ')' { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } statement { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
;


new_elseif_list:
		/* empty */
	|	new_elseif_list T_ELSEIF '(' expr ')' ':' { zend_do_if_cond(&$4, &$5 TSRMLS_CC); } inner_statement_list { zend_do_if_after_statement(&$5, 0 TSRMLS_CC); }
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
		T_VARIABLE				{ znode tmp;  fetch_simple_variable(&tmp, &$1, 0 TSRMLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE TSRMLS_CC); }
	|	'&' T_VARIABLE			{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 TSRMLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_FORCE TSRMLS_CC); }
	|	'&' T_VARIABLE '=' static_scalar			{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 TSRMLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$4, BYREF_FORCE TSRMLS_CC); }
	|	T_CONST T_VARIABLE 		{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 TSRMLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE TSRMLS_CC); }
	|	T_VARIABLE '=' static_scalar				{ znode tmp;  fetch_simple_variable(&tmp, &$1, 0 TSRMLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; INIT_PZVAL(&$$.u.constant); zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$3, BYREF_NONE TSRMLS_CC); }
	|	non_empty_parameter_list ',' T_VARIABLE 	{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 TSRMLS_CC); $$=$1; $$.u.constant.value.lval++; zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE TSRMLS_CC); }
	|	non_empty_parameter_list ',' '&' T_VARIABLE	{ znode tmp;  fetch_simple_variable(&tmp, &$4, 0 TSRMLS_CC); $$=$1; $$.u.constant.value.lval++; zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_FORCE TSRMLS_CC); }
	|	non_empty_parameter_list ',' '&' T_VARIABLE	 '=' static_scalar { znode tmp;  fetch_simple_variable(&tmp, &$4, 0 TSRMLS_CC); $$=$1; $$.u.constant.value.lval++; zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$6, BYREF_FORCE TSRMLS_CC); }
	|	non_empty_parameter_list ',' T_CONST T_VARIABLE				{ znode tmp;  fetch_simple_variable(&tmp, &$4, 0 TSRMLS_CC); $$=$1; $$.u.constant.value.lval++; zend_do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE TSRMLS_CC); }
	|	non_empty_parameter_list ',' T_VARIABLE '=' static_scalar 	{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 TSRMLS_CC); $$=$1; $$.u.constant.value.lval++; zend_do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$5, BYREF_NONE TSRMLS_CC); }
;


function_call_parameter_list:
		non_empty_function_call_parameter_list	{ $$ = $1; }
	|	/* empty */				{ $$.u.constant.value.lval = 0; }
;


non_empty_function_call_parameter_list:
		expr_without_variable	{ $$.u.constant.value.lval = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAL, $$.u.constant.value.lval TSRMLS_CC); }
	|	variable				{ $$.u.constant.value.lval = 1;  zend_do_pass_param(&$1, ZEND_SEND_VAR, $$.u.constant.value.lval TSRMLS_CC); }
	|	'&' w_variable 				{ $$.u.constant.value.lval = 1;  zend_do_pass_param(&$2, ZEND_SEND_REF, $$.u.constant.value.lval TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' expr_without_variable	{ $$.u.constant.value.lval=$1.u.constant.value.lval+1;  zend_do_pass_param(&$3, ZEND_SEND_VAL, $$.u.constant.value.lval TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' variable					{ $$.u.constant.value.lval=$1.u.constant.value.lval+1;  zend_do_pass_param(&$3, ZEND_SEND_VAR, $$.u.constant.value.lval TSRMLS_CC); }
	|	non_empty_function_call_parameter_list ',' '&' w_variable				{ $$.u.constant.value.lval=$1.u.constant.value.lval+1;  zend_do_pass_param(&$4, ZEND_SEND_REF, $$.u.constant.value.lval TSRMLS_CC); }
;

global_var_list:
		global_var_list ',' global_var	{ zend_do_fetch_global_or_static_variable(&$3, NULL, ZEND_FETCH_GLOBAL TSRMLS_CC); }
	|	global_var						{ zend_do_fetch_global_or_static_variable(&$1, NULL, ZEND_FETCH_GLOBAL TSRMLS_CC); }
;


global_var:
		T_VARIABLE			{ $$ = $1; }
	|	'$' r_variable			{ $$ = $2; }
	|	'$' '{' expr '}'	{ $$ = $3; }
;


static_var_list:
		static_var_list ',' T_VARIABLE { zend_do_fetch_global_or_static_variable(&$3, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	static_var_list ',' T_VARIABLE '=' static_scalar { zend_do_fetch_global_or_static_variable(&$3, &$5, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE  { zend_do_fetch_global_or_static_variable(&$1, NULL, ZEND_FETCH_STATIC TSRMLS_CC); }
	|	T_VARIABLE '=' static_scalar { zend_do_fetch_global_or_static_variable(&$1, &$3, ZEND_FETCH_STATIC TSRMLS_CC); }

;


class_statement_list:
		class_statement_list class_statement
	|	/* empty */
;


class_statement:
		class_variable_declaration ';'
	|	class_constant_declaration ';'
	|	T_FUNCTION { $1.u.opline_num = CG(zend_lineno); } is_reference T_STRING { zend_do_begin_function_declaration(&$1, &$4, 1, $3.op_type TSRMLS_CC); } '(' 
			parameter_list ')' '{' inner_statement_list '}' { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
	|	T_OLD_FUNCTION { $1.u.opline_num = CG(zend_lineno); } is_reference T_STRING { zend_do_begin_function_declaration(&$1, &$4, 1, $3.op_type TSRMLS_CC); }
			parameter_list '(' inner_statement_list ')' ';' { zend_do_end_function_declaration(&$1 TSRMLS_CC); }
	|	T_CLASS T_STRING { zend_do_begin_class_declaration(&$1, &$2, NULL TSRMLS_CC); } '{' class_statement_list '}' { zend_do_end_class_declaration(&$1 TSRMLS_CC); }
	|	T_CLASS T_STRING T_EXTENDS T_STRING { zend_do_begin_class_declaration(&$1, &$2, &$4 TSRMLS_CC); } '{' class_statement_list '}' { zend_do_end_class_declaration(&$1 TSRMLS_CC); }
;

is_reference:
		/* empty */	{ $$.op_type = ZEND_RETURN_VAL; }
	|	'&'			{ $$.op_type = ZEND_RETURN_REF; }
;

class_variable_declaration:
		class_variable_declaration ',' T_VARIABLE					{ zend_do_declare_property(&$3, NULL, $1.op_type TSRMLS_CC); }
	|	class_variable_declaration ',' T_VARIABLE '=' static_scalar	{ zend_do_declare_property(&$3, &$5, $1.op_type TSRMLS_CC); }
	|	class_declaration_type T_VARIABLE						{ $$ = $1; zend_do_declare_property(&$2, NULL, $1.op_type TSRMLS_CC); }
	|	class_declaration_type T_VARIABLE '=' static_scalar	{ $$ = $1; zend_do_declare_property(&$2, &$4, $1.op_type TSRMLS_CC); }
;

class_declaration_type:
		T_VAR		{ $$.op_type = T_VAR; }
	|	T_STATIC	{ $$.op_type = T_STATIC; }
	|	T_PRIVATE	{ $$.op_type = T_PRIVATE; }
	|	T_PROTECTED	{ $$.op_type = T_PROTECTED; }
;

class_constant_declaration:
	|	T_CONST ',' T_STRING '=' static_scalar	{ zend_do_declare_property(&$3, &$5, T_CONST TSRMLS_CC); }
	|	T_CONST T_STRING '=' static_scalar	{ zend_do_declare_property(&$2, &$4, T_CONST TSRMLS_CC); }
;

echo_expr_list:	
	|	echo_expr_list ',' expr { zend_do_echo(&$3 TSRMLS_CC); }
	|	expr					{ zend_do_echo(&$1 TSRMLS_CC); }
;


for_expr:
		/* empty */			{ $$.op_type = IS_CONST;  $$.u.constant.type = IS_BOOL;  $$.u.constant.value.lval = 1; }
	|	non_empty_for_expr	{ $$ = $1; }
;

non_empty_for_expr:
		non_empty_for_expr ','	{ zend_do_free(&$1 TSRMLS_CC); } expr { $$ = $4; }
	|	expr					{ $$ = $1; }
;

expr_without_variable:	
		T_LIST '(' { zend_do_list_init(TSRMLS_C); } assignment_list ')' '=' expr { zend_do_list_end(&$$, &$7 TSRMLS_CC); }
	|	variable '=' expr		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC); zend_do_assign(&$$, &$1, &$3 TSRMLS_CC); }
	|	variable '=' '&' variable { zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC); zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&$$, &$1, &$4 TSRMLS_CC); }
	|	variable '=' '&' T_NEW new_class_entry { zend_check_writable_variable(&$$); zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$4, &$5 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$3, &$4, &$7 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC); zend_do_assign_ref(&$$, &$1, &$3 TSRMLS_CC); }
	|	T_NEW new_class_entry { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_new_object(&$1, &$2 TSRMLS_CC); } ctor_arguments { zend_do_end_new_object(&$$, &$1, &$4 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	variable T_PLUS_EQUAL expr 	{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_ADD, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MINUS_EQUAL expr	{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SUB, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MUL_EQUAL expr		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MUL, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_DIV_EQUAL expr		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_DIV, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_CONCAT_EQUAL expr	{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_CONCAT, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_MOD_EQUAL expr		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_MOD, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_AND_EQUAL expr		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_AND, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_OR_EQUAL expr 		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_OR, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_XOR_EQUAL expr 		{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &$$, &$1, &$3 TSRMLS_CC); }
	|	variable T_SL_EQUAL expr	{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SL, &$$, &$1, &$3 TSRMLS_CC); } 
	|	variable T_SR_EQUAL expr	{ zend_check_writable_variable(&$$); zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); zend_do_binary_assign_op(ZEND_ASSIGN_SR, &$$, &$1, &$3 TSRMLS_CC); } 
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
	|	'+' expr { $1.u.constant.value.lval=0; $1.u.constant.type=IS_LONG; $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_ADD, &$$, &$1, &$2 TSRMLS_CC); }
	|	'-' expr { $1.u.constant.value.lval=0; $1.u.constant.type=IS_LONG; $1.op_type = IS_CONST; INIT_PZVAL(&$1.u.constant); zend_do_binary_op(ZEND_SUB, &$$, &$1, &$2 TSRMLS_CC); }
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
	|	'(' expr ')' 	{ $$ = $2; }
	|	expr '?' { zend_do_begin_qm_op(&$1, &$2 TSRMLS_CC); }
		expr ':' { zend_do_qm_true(&$4, &$2, &$5 TSRMLS_CC); }
		expr	 { zend_do_qm_false(&$$, &$7, &$2, &$5 TSRMLS_CC); }
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
	|	T_ARRAY '(' array_pair_list ')' { $$ = $3; }
	|	'`' encaps_list '`'		{ zend_do_shell_exec(&$$, &$2 TSRMLS_CC); }
	|	T_PRINT expr  { zend_do_print(&$$, &$2 TSRMLS_CC); }
;

function_call:
		T_STRING	'(' { $2.u.opline_num = zend_do_begin_function_call(&$1 TSRMLS_CC); }
				function_call_parameter_list
				')' { zend_do_end_function_call(&$1, &$$, &$4, 0, $2.u.opline_num TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C); }
	|	parse_class_entry static_or_variable_string '(' { zend_do_extended_fcall_begin(TSRMLS_C); zend_do_begin_class_member_function_call(&$1, &$2 TSRMLS_CC); } 
											function_call_parameter_list 
											')' { zend_do_end_function_call(&$2, &$$, &$5, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
	|	variable_without_objects  '(' { zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC); zend_do_begin_dynamic_function_call(&$1 TSRMLS_CC); }
			function_call_parameter_list ')'
			{ zend_do_end_function_call(&$1, &$$, &$4, 0, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);}
;

parse_class_entry:
		parse_class_entry T_STRING T_PAAMAYIM_NEKUDOTAYIM { do_fetch_class(&$$, &$1, &$2 TSRMLS_CC); }
	|	T_STRING T_PAAMAYIM_NEKUDOTAYIM { do_fetch_class(&$$, NULL, &$1 TSRMLS_CC); }
;

parse_class_name_entry:
		parse_class_name_entry T_STRING T_PAAMAYIM_NEKUDOTAYIM { do_fetch_class_name(&$$, &$1, &$2, 0 TSRMLS_CC); }
	|	T_STRING T_PAAMAYIM_NEKUDOTAYIM { $$ = $1; zend_str_tolower($$.u.constant.value.str.val, $$.u.constant.value.str.len); }
;

catch_or_import_class_entry:
		parse_class_entry T_STRING { do_fetch_class(&$$, &$1, &$2 TSRMLS_CC); }
	|	T_STRING { do_fetch_class(&$$, NULL, &$1 TSRMLS_CC); }
;

new_class_entry:
		parse_class_entry T_STRING { do_fetch_class(&$$, &$1, &$2 TSRMLS_CC); }
	|	static_or_variable_string { do_fetch_class(&$$, NULL, &$1 TSRMLS_CC); }
;

static_or_variable_string:
		T_STRING	{ $$ = $1; }
	|	r_variable_without_static_member	{ $$ = $1; }
;


exit_expr:
		/* empty */	{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }	
	|	'(' ')'		{ memset(&$$, 0, sizeof(znode)); $$.op_type = IS_UNUSED; }	
	|	'(' expr ')'	{ $$ = $2; }
;


ctor_arguments:
		/* empty */	{ $$.u.constant.value.lval=0; }
	|	'(' function_call_parameter_list ')'	{ $$ = $2; }
;


common_scalar:
		T_LNUMBER 					{ $$ = $1; }
	|	T_DNUMBER 					{ $$ = $1; }
	|	T_CONSTANT_ENCAPSED_STRING	{ $$ = $1; }
	|	T_LINE 						{ $$ = $1; }
	|	T_FILE 						{ $$ = $1; }
	|	T_CLASS_C					{ $$ = $1; }
	|	T_FUNC_C					{ $$ = $1; }
;


static_scalar: /* compile-time evaluated scalars */
		common_scalar		{ $$ = $1; }
	|	T_STRING 		{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_CT TSRMLS_CC); }
	|	'+' static_scalar	{ $$ = $2; }
	|	'-' static_scalar	{ zval minus_one;  minus_one.type = IS_LONG; minus_one.value.lval = -1;  mul_function(&$2.u.constant, &$2.u.constant, &minus_one TSRMLS_CC);  $$ = $2; }
	|	T_ARRAY '(' static_array_pair_list ')' { $$ = $3; $$.u.constant.type = IS_CONSTANT_ARRAY; }
	|	parse_class_name_entry T_STRING { zend_do_fetch_constant(&$$, &$1, &$2, ZEND_CT TSRMLS_CC); }
;


scalar:
		T_STRING 				{ zend_do_fetch_constant(&$$, NULL, &$1, ZEND_RT TSRMLS_CC); }
	|	T_STRING_VARNAME		{ $$ = $1; }
	|	parse_class_entry T_STRING { zend_do_fetch_constant(&$$, &$1, &$2, ZEND_RT TSRMLS_CC); }
	|	common_scalar			{ $$ = $1; }
	|	'"' encaps_list '"' 	{ $$ = $2; }
	|	'\'' encaps_list '\''	{ $$ = $2; }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$ = $2; zend_do_end_heredoc(TSRMLS_C); }
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


r_variable:
	variable { zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC); $$ = $1; }
;


w_variable:
	variable	{ zend_do_end_variable_parse(BP_VAR_W, 0 TSRMLS_CC); $$ = $1; }
				{ zend_check_writable_variable(&$$); }
;

rw_variable:
	variable	{ zend_do_end_variable_parse(BP_VAR_RW, 0 TSRMLS_CC); $$ = $1; }
				{ zend_check_writable_variable(&$$); }
;

r_variable_without_static_member:
		variable_without_objects { zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC); $$ = $1; }
;

variable:
		base_variable_without_objects T_OBJECT_OPERATOR { zend_do_push_object(&$1 TSRMLS_CC); }
			object_property { zend_do_push_object(&$4 TSRMLS_CC); } method_or_not variable_properties
			{ zend_do_pop_object(&$$ TSRMLS_CC); $$.u.EA.type = $1.u.EA.type | ($7.u.EA.type ? $7.u.EA.type : $6.u.EA.type); }
	|	base_variable_without_objects { $$ = $1; }
;


variable_properties:
		variable_properties variable_property { $$.u.EA.type = $2.u.EA.type; }
	|	/* empty */ { $$.u.EA.type = 0; }
;


variable_property:
		T_OBJECT_OPERATOR object_property { zend_do_push_object(&$2 TSRMLS_CC); } method_or_not { $$.u.EA.type = $4.u.EA.type; }
;

method_or_not:
		'(' { zend_do_pop_object(&$1 TSRMLS_CC); zend_do_begin_method_call(&$1 TSRMLS_CC); }
				function_call_parameter_list ')' 
			{ zend_do_end_function_call(&$1, &$$, &$3, 1, 1 TSRMLS_CC); zend_do_extended_fcall_end(TSRMLS_C);
			  zend_do_push_object(&$$ TSRMLS_CC); $$.u.EA.type = ZEND_PARSED_METHOD_CALL; }
	|	/* empty */ { $$.u.EA.type = ZEND_PARSED_MEMBER; }
;

variable_without_objects:
		reference_variable { $$ = $1; }
	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); }
;

static_member:
	parse_class_entry variable_without_objects { $$ = $2; zend_do_fetch_static_member(&$1 TSRMLS_CC); }
;


base_variable_without_objects:
		reference_variable { $$ = $1; $$.u.EA.type = ZEND_PARSED_VARIABLE; }
	|	simple_indirect_reference reference_variable { zend_do_indirect_references(&$$, &$1, &$2 TSRMLS_CC); $$.u.EA.type = ZEND_PARSED_VARIABLE; }
	|	static_member { $$ = $1; $$.u.EA.type = ZEND_PARSED_STATIC_MEMBER; }
	|	function_call { zend_do_begin_variable_parse(TSRMLS_C); $$ = $1; $$.u.EA.type = ZEND_PARSED_FUNCTION_CALL; }
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
	|	variable_without_objects { zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC); } { znode tmp_znode;  zend_do_pop_object(&tmp_znode TSRMLS_CC);  zend_do_fetch_property(&$$, &tmp_znode, &$1 TSRMLS_CC);}
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
		'$' { $$.u.constant.value.lval = 1; }
	|	simple_indirect_reference '$' { $$.u.constant.value.lval++; }
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
		encaps_list encaps_var { zend_do_end_variable_parse(BP_VAR_R, 0 TSRMLS_CC);  zend_do_add_variable(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_STRING			{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_NUM_STRING		{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE	{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_CHARACTER 		{ zend_do_add_char(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_BAD_CHARACTER		{ zend_do_add_string(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list '['		{ $2.u.constant.value.lval = (long) '['; zend_do_add_char(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list ']'		{ $2.u.constant.value.lval = (long) ']'; zend_do_add_char(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list '{'		{ $2.u.constant.value.lval = (long) '{'; zend_do_add_char(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list '}'		{ $2.u.constant.value.lval = (long) '}'; zend_do_add_char(&$$, &$1, &$2 TSRMLS_CC); }
	|	encaps_list T_OBJECT_OPERATOR  { znode tmp;  $2.u.constant.value.lval = (long) '-';  zend_do_add_char(&tmp, &$1, &$2 TSRMLS_CC);  $2.u.constant.value.lval = (long) '>'; zend_do_add_char(&$$, &tmp, &$2 TSRMLS_CC); }
	|	/* empty */			{ zend_do_init_string(&$$ TSRMLS_CC); }

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
	|	T_INCLUDE expr 			{ zend_do_include_or_eval(ZEND_INCLUDE, &$$, &$2 TSRMLS_CC); }
	|	T_INCLUDE_ONCE expr 	{ zend_do_include_or_eval(ZEND_INCLUDE_ONCE, &$$, &$2 TSRMLS_CC); }
	|	T_EVAL '(' expr ')' 	{ zend_do_include_or_eval(ZEND_EVAL, &$$, &$3 TSRMLS_CC); }
	|	T_REQUIRE expr			{ zend_do_include_or_eval(ZEND_REQUIRE, &$$, &$2 TSRMLS_CC); }
	|	T_REQUIRE_ONCE expr		{ zend_do_include_or_eval(ZEND_REQUIRE_ONCE, &$$, &$2 TSRMLS_CC); }
;

isset_variables:
		variable 				{ zend_do_isset_or_isempty(ZEND_ISSET, &$$, &$1 TSRMLS_CC); }
	|	isset_variables ',' { zend_do_boolean_and_begin(&$1, &$2 TSRMLS_CC); } variable { znode tmp; zend_do_isset_or_isempty(ZEND_ISSET, &tmp, &$4 TSRMLS_CC); zend_do_boolean_and_end(&$$, &$1, &tmp, &$2 TSRMLS_CC); }
;	

%%

