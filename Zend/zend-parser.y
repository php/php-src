%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
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
#include "zend_variables.h"
#include "zend_operators.h"


#define YYERROR_VERBOSE

%}

%pure_parser
%expect 4

%left INCLUDE EVAL
%left ','
%left LOGICAL_OR
%left LOGICAL_XOR
%left LOGICAL_AND
%right ZEND_PRINT
%left '=' PLUS_EQUAL MINUS_EQUAL MUL_EQUAL DIV_EQUAL CONCAT_EQUAL MOD_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL SHIFT_LEFT_EQUAL SHIFT_RIGHT_EQUAL
%left '?' ':'
%left BOOLEAN_OR
%left BOOLEAN_AND
%left '|'
%left '^'
%left '&'
%nonassoc IS_EQUAL IS_NOT_EQUAL
%nonassoc '<' IS_SMALLER_OR_EQUAL '>' IS_GREATER_OR_EQUAL
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-' '.'
%left '*' '/' '%'
%right '!' '~' INCREMENT DECREMENT INT_CAST DOUBLE_CAST STRING_CAST ARRAY_CAST OBJECT_CAST '@'
%right '['
%nonassoc NEW
%token T_EXIT
%token IF
%left ELSEIF
%left ELSE
%left ENDIF
%token LNUMBER
%token DNUMBER
%token STRING
%token VARIABLE
%token NUM_STRING
%token INLINE_HTML
%token CHARACTER
%token BAD_CHARACTER
%token ENCAPSED_AND_WHITESPACE
%token CONSTANT_ENCAPSED_STRING
%token ZEND_ECHO
%token DO
%token WHILE
%token ENDWHILE
%token FOR
%token ENDFOR
%token ZEND_FOREACH
%token T_ENDFOREACH
%token ZEND_AS
%token SWITCH
%token ENDSWITCH
%token CASE
%token DEFAULT
%token BREAK
%token CONTINUE
%token CONTINUED_WHILE
%token CONTINUED_DOWHILE
%token CONTINUED_FOR
%token OLD_FUNCTION
%token FUNCTION
%token ZEND_CONST
%token RETURN
%token REQUIRE
%token ZEND_GLOBAL
%token ZEND_STATIC
%token ZEND_UNSET
%token T_ISSET
%token T_EMPTY
%token CLASS
%token EXTENDS
%token ZEND_OBJECT_OPERATOR
%token ZEND_DOUBLE_ARROW
%token ZEND_LIST
%token ZEND_ARRAY
%token VAR
%token ZEND_LINE
%token ZEND_FILE
%token ZEND_COMMENT
%token ZEND_ML_COMMENT
%token PHP_OPEN_TAG
%token PHP_OPEN_TAG_WITH_ECHO
%token PHP_CLOSE_TAG
%token T_WHITESPACE
%token ZEND_HEREDOC
%token DOLLAR_OPEN_CURLY_BRACES
%token T_CURLY_OPEN
%token T_PAAMAYIM_NEKUDOTAYIM
%token T_PHP_TRACK_VARS

%% /* Rules */

statement_list:	
		statement_list  { do_extended_info(CLS_C); } statement { ELS_FETCH(); HANDLE_INTERACTIVE(); }
	|	/* empty */
;


statement:
		'{' statement_list '}'
	|	IF '(' expr ')' { do_if_cond(&$3, &$4 CLS_CC); } statement { do_if_after_statement(&$4, 1 CLS_CC); } elseif_list else_single { do_if_end(CLS_C); }
	|	IF '(' expr ')' ':' { do_if_cond(&$3, &$4 CLS_CC); } statement_list { do_if_after_statement(&$4, 1 CLS_CC); } new_elseif_list new_else_single ENDIF ';' { do_if_end(CLS_C); }
	|	WHILE '(' { $1.u.opline_num = get_next_op_number(CG(active_op_array));  } expr  ')' { do_while_cond(&$4, &$5 CLS_CC); } while_statement { do_while_end(&$1, &$5 CLS_CC); }
	|	DO { $1.u.opline_num = get_next_op_number(CG(active_op_array));  do_do_while_begin(CLS_C); } statement WHILE '(' expr ')' ';' { do_do_while_end(&$1, &$6 CLS_CC); }
	|	FOR 
			'('
				for_expr
			';' { do_free(&$3 CLS_CC); $4.u.opline_num = get_next_op_number(CG(active_op_array)); }
				for_expr
			';' { do_for_cond(&$6, &$7 CLS_CC); }
				for_expr
			')' { do_free(&$9 CLS_CC); do_for_before_statement(&$4, &$7 CLS_CC); }
			for_statement { do_for_end(&$7 CLS_CC); }
	|	SWITCH '(' expr ')' { do_switch_cond(&$3 CLS_CC); } switch_case_list { do_switch_end(&$6 CLS_CC); }
	|	BREAK ';'		 	{ do_brk_cont(ZEND_BRK, NULL CLS_CC); }
	|	BREAK expr ';'		{ do_brk_cont(ZEND_BRK, &$2 CLS_CC); }
	|	CONTINUE ';'		{ do_brk_cont(ZEND_CONT, NULL CLS_CC); }
	|	CONTINUE expr ';'	{ do_brk_cont(ZEND_CONT, &$2 CLS_CC); }
	|	FUNCTION { $1.u.opline_num = CG(zend_lineno); } STRING { do_begin_function_declaration(&$1, &$3, 0 CLS_CC); }
			'(' parameter_list ')' '{' statement_list '}' { do_end_function_declaration(&$1 CLS_CC); }
	|	OLD_FUNCTION { $1.u.opline_num = CG(zend_lineno); } STRING  { do_begin_function_declaration(&$1, &$3, 0 CLS_CC); }
			parameter_list '(' statement_list ')' ';' { do_end_function_declaration(&$1 CLS_CC); }
	|	RETURN ';'			{ do_return(NULL CLS_CC); }
	|	RETURN expr ';'		{ do_return(&$2 CLS_CC); }
	|	ZEND_GLOBAL global_var_list
	|	ZEND_STATIC static_var_list
	|	CLASS STRING { do_begin_class_declaration(&$2, NULL CLS_CC); } '{' class_statement_list '}' { do_end_class_declaration(CLS_C); }
	|	CLASS STRING EXTENDS STRING { do_begin_class_declaration(&$2, &$4 CLS_CC); } '{' class_statement_list '}' { do_end_class_declaration(CLS_C); }
	|	ZEND_ECHO echo_expr_list ';'
	|	INLINE_HTML			{ do_echo(&$1 CLS_CC); }
	|	expr ';'			{ do_free(&$1 CLS_CC); }
	|	REQUIRE CONSTANT_ENCAPSED_STRING ';'			{ require_filename($2.u.constant.value.str.val CLS_CC); zval_dtor(&$2.u.constant); }
	|	REQUIRE '(' CONSTANT_ENCAPSED_STRING ')' ';'	{ require_filename($3.u.constant.value.str.val CLS_CC); zval_dtor(&$3.u.constant); }
	|	ZEND_UNSET '(' r_cvar ')' ';' { do_unset(&$3 CLS_CC); }
	|	ZEND_FOREACH '(' expr ZEND_AS { do_foreach_begin(&$1, &$3, &$2, &$4 CLS_CC); } w_cvar foreach_optional_arg ')' { do_foreach_cont(&$6, &$7, &$4 CLS_CC); } foreach_statement { do_foreach_end(&$1, &$2 CLS_CC); }
	|	';'		/* empty statement */
;


foreach_optional_arg:
		/* empty */		{ $$.op_type = IS_UNUSED; }
	|	ZEND_DOUBLE_ARROW w_cvar		{ $$ = $2; }
;


for_statement:
		statement
	|	':' statement_list ENDFOR ';'
;


foreach_statement:
		statement
	|	':' statement_list T_ENDFOREACH ';'
;


switch_case_list:
		'{' case_list '}'					{ $$ = $2; }
	|	'{' ';' case_list '}'				{ $$ = $3; }
	|	':' case_list ENDSWITCH ';'			{ $$ = $2; }
	|	':' ';' case_list ENDSWITCH ';'		{ $$ = $3; }
;


case_list:
		/* empty */	{ $$.u.opline_num = -1; }
	|	case_list CASE expr case_separator { do_case_before_statement(&$1, &$2, &$3 CLS_CC); } statement_list { do_case_after_statement(&$$, &$2 CLS_CC); }
	|	case_list DEFAULT case_separator { do_default_before_statement(&$1, &$2 CLS_CC); } statement_list { do_case_after_statement(&$$, &$2 CLS_CC); }
;


case_separator:
		':'
	|	';'
;


while_statement:
		statement
	|	':' statement_list ENDWHILE ';'
;



elseif_list:
		/* empty */
	|	elseif_list ELSEIF '(' expr ')' { do_if_cond(&$4, &$5 CLS_CC); } statement { do_if_after_statement(&$5, 0 CLS_CC); }
;


new_elseif_list:
		/* empty */
	|	new_elseif_list ELSEIF '(' expr ')' ':' { do_if_cond(&$4, &$5 CLS_CC); } statement_list { do_if_after_statement(&$5, 0 CLS_CC); }
;


else_single:
		/* empty */
	|	ELSE statement
;


new_else_single:
		/* empty */
	|	ELSE ':' statement_list
;




parameter_list: 
		non_empty_parameter_list
	|	/* empty */
;


non_empty_parameter_list:
		VARIABLE						{ znode tmp;  fetch_simple_variable(&tmp, &$1, 0 CLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
	|	'&' VARIABLE					{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 CLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_FORCE CLS_CC); }
	|	ZEND_CONST VARIABLE 			{ znode tmp;  fetch_simple_variable(&tmp, &$2, 0 CLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
	|	VARIABLE '=' static_scalar		{ znode tmp;  fetch_simple_variable(&tmp, &$1, 0 CLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$3, BYREF_NONE CLS_CC); }
	|	VARIABLE '=' ZEND_UNSET 		{ znode tmp;  fetch_simple_variable(&tmp, &$1, 0 CLS_CC); $$.op_type = IS_CONST; $$.u.constant.value.lval=1; $$.u.constant.type=IS_LONG; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
	|	non_empty_parameter_list ',' VARIABLE 						{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 CLS_CC); $$=$1; $$.u.constant.value.lval++; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
	|	non_empty_parameter_list ',' '&' VARIABLE					{ znode tmp;  fetch_simple_variable(&tmp, &$4, 0 CLS_CC); $$=$1; $$.u.constant.value.lval++; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_FORCE CLS_CC); }
	|	non_empty_parameter_list ',' ZEND_CONST VARIABLE			{ znode tmp;  fetch_simple_variable(&tmp, &$4, 0 CLS_CC); $$=$1; $$.u.constant.value.lval++; do_receive_arg(ZEND_RECV, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
	|	non_empty_parameter_list ',' VARIABLE '=' static_scalar 	{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 CLS_CC); $$=$1; $$.u.constant.value.lval++; do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, &$5, BYREF_NONE CLS_CC); }
	|	non_empty_parameter_list ',' VARIABLE '=' ZEND_UNSET	 	{ znode tmp;  fetch_simple_variable(&tmp, &$3, 0 CLS_CC); $$=$1; $$.u.constant.value.lval++; do_receive_arg(ZEND_RECV_INIT, &tmp, &$$, NULL, BYREF_NONE CLS_CC); }
;


function_call_parameter_list:
		non_empty_function_call_parameter_list		{ $$ = $1; }
	|	/* empty */									{ $$.u.constant.value.lval = 0; }
;


non_empty_function_call_parameter_list:
		expr_without_variable	{	$$.u.constant.value.lval = 1;  do_pass_param(&$1, ZEND_SEND_VAL, $$.u.constant.value.lval CLS_CC); }
	|	cvar					{	$$.u.constant.value.lval = 1;  do_pass_param(&$1, ZEND_SEND_VAR, $$.u.constant.value.lval CLS_CC); }
	|	'&' w_cvar 				{	$$.u.constant.value.lval = 1;  do_pass_param(&$2, ZEND_SEND_REF, $$.u.constant.value.lval CLS_CC); }
	|	non_empty_function_call_parameter_list ',' expr_without_variable	{	$$.u.constant.value.lval=$1.u.constant.value.lval+1;  do_pass_param(&$3, ZEND_SEND_VAL, $$.u.constant.value.lval CLS_CC); }
	|	non_empty_function_call_parameter_list ',' cvar 					{	$$.u.constant.value.lval=$1.u.constant.value.lval+1;  do_pass_param(&$3, ZEND_SEND_VAR, $$.u.constant.value.lval CLS_CC); }
	|	non_empty_function_call_parameter_list ',' '&' w_cvar				{	$$.u.constant.value.lval=$1.u.constant.value.lval+1;  do_pass_param(&$4, ZEND_SEND_REF, $$.u.constant.value.lval CLS_CC); }
;

global_var_list:
		global_var_list ',' global_var { do_fetch_global_or_static_variable(&$3, NULL, ZEND_FETCH_GLOBAL CLS_CC); }
	|	global_var { do_fetch_global_or_static_variable(&$1, NULL, ZEND_FETCH_GLOBAL CLS_CC); }
;


global_var:
		VARIABLE			{ $$ = $1; }
	|	'$' r_cvar			{ $$ = $2; }
	|	'$' '{' expr '}'	{ $$ = $3; }
;


static_var_list:
		static_var_list ',' VARIABLE { do_fetch_global_or_static_variable(&$3, NULL, ZEND_FETCH_STATIC CLS_CC); }
	|	static_var_list ',' VARIABLE '=' static_scalar { do_fetch_global_or_static_variable(&$3, &$5, ZEND_FETCH_STATIC CLS_CC); }
	|	VARIABLE  { do_fetch_global_or_static_variable(&$1, NULL, ZEND_FETCH_STATIC CLS_CC); }
	|	VARIABLE '=' static_scalar { do_fetch_global_or_static_variable(&$1, &$3, ZEND_FETCH_STATIC CLS_CC); }

;


class_statement_list:
		class_statement_list class_statement
	|	/* empty */
;


class_statement:
		VAR class_variable_decleration ';'
	|	FUNCTION { $1.u.opline_num = CG(zend_lineno); } STRING { do_begin_function_declaration(&$1, &$3, 1 CLS_CC); } '(' 
			parameter_list ')' '{' statement_list '}' { do_end_function_declaration(&$1 CLS_CC); }
	|	OLD_FUNCTION { $1.u.opline_num = CG(zend_lineno); } STRING { do_begin_function_declaration(&$1, &$3, 1 CLS_CC); }
			parameter_list '(' statement_list ')' ';' { do_end_function_declaration(&$1 CLS_CC); }

;


class_variable_decleration:
		class_variable_decleration ',' VARIABLE { do_declare_property(&$3, NULL CLS_CC); }
	|	class_variable_decleration ',' VARIABLE '=' static_scalar  { do_declare_property(&$3, &$5 CLS_CC); }
	|	VARIABLE	{ do_declare_property(&$1, NULL CLS_CC); }
	|	VARIABLE '=' static_scalar { do_declare_property(&$1, &$3 CLS_CC); }
;

	
echo_expr_list:	
		/* empty */
	|	echo_expr_list ',' expr { do_echo(&$3 CLS_CC); }
	|	expr					{ do_echo(&$1 CLS_CC); }
;


for_expr:
		/* empty */			{ $$.op_type = IS_CONST;  $$.u.constant.type = IS_BOOL;  $$.u.constant.value.lval = 1; }
	|	for_expr ',' expr	{ $$ = $3; }
	|	expr				{ $$ = $1; }
;


expr_without_variable:	
		ZEND_LIST '(' { do_list_init(); } assignment_list ')' '=' expr { do_list_end(&$$, &$7 CLS_CC); }
	|	w_cvar '=' expr		{ do_assign(&$$, &$1, &$3 CLS_CC); }
	|	w_cvar '=' '&' w_cvar	{ do_assign_ref(&$$, &$1, &$4 CLS_CC); }
	|	w_cvar '=' NEW class_name { do_extended_fcall_begin(CLS_C); do_begin_new_object(&$2, &$1, &$3, &$4 CLS_CC); } ctor_arguments { do_end_new_object(&$4, &$3, &$6 CLS_CC); do_extended_fcall_end(CLS_C);  $$ = $2;}
	|	rw_cvar PLUS_EQUAL expr 	{ do_binary_assign_op(ZEND_ASSIGN_ADD, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar MINUS_EQUAL expr	{ do_binary_assign_op(ZEND_ASSIGN_SUB, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar MUL_EQUAL expr		{ do_binary_assign_op(ZEND_ASSIGN_MUL, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar DIV_EQUAL expr		{ do_binary_assign_op(ZEND_ASSIGN_DIV, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar CONCAT_EQUAL expr	{ do_binary_assign_op(ZEND_ASSIGN_CONCAT, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar MOD_EQUAL expr		{ do_binary_assign_op(ZEND_ASSIGN_MOD, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar AND_EQUAL expr		{ do_binary_assign_op(ZEND_ASSIGN_BW_AND, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar OR_EQUAL expr 		{ do_binary_assign_op(ZEND_ASSIGN_BW_OR, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar XOR_EQUAL expr 		{ do_binary_assign_op(ZEND_ASSIGN_BW_XOR, &$$, &$1, &$3 CLS_CC); }
	|	rw_cvar SHIFT_LEFT_EQUAL expr	{ do_binary_assign_op(ZEND_ASSIGN_SL, &$$, &$1, &$3 CLS_CC); } 
	|	rw_cvar SHIFT_RIGHT_EQUAL expr	{ do_binary_assign_op(ZEND_ASSIGN_SR, &$$, &$1, &$3 CLS_CC); } 
	|	rw_cvar INCREMENT { do_post_incdec(&$$, &$1, ZEND_POST_INC CLS_CC); }
	|	INCREMENT rw_cvar { do_pre_incdec(&$$, &$2, ZEND_PRE_INC CLS_CC); }
	|	rw_cvar DECREMENT { do_post_incdec(&$$, &$1, ZEND_POST_DEC CLS_CC); }
	|	DECREMENT rw_cvar { do_pre_incdec(&$$, &$2, ZEND_PRE_DEC CLS_CC); }
	|	expr BOOLEAN_OR { do_boolean_or_begin(&$1, &$2 CLS_CC); } expr { do_boolean_or_end(&$$, &$1, &$4, &$2 CLS_CC); }
	|	expr BOOLEAN_AND { do_boolean_and_begin(&$1, &$2 CLS_CC); } expr { do_boolean_and_end(&$$, &$1, &$4, &$2 CLS_CC); }  
	|	expr LOGICAL_OR { do_boolean_or_begin(&$1, &$2 CLS_CC); } expr { do_boolean_or_end(&$$, &$1, &$4, &$2 CLS_CC); }
	|	expr LOGICAL_AND { do_boolean_and_begin(&$1, &$2 CLS_CC); } expr { do_boolean_and_end(&$$, &$1, &$4, &$2 CLS_CC); }
	|	expr LOGICAL_XOR expr { do_binary_op(ZEND_BOOL_XOR, &$$, &$1, &$3 CLS_CC); }
	|	expr '|' expr	{ do_binary_op(ZEND_BW_OR, &$$, &$1, &$3 CLS_CC); }
	|	expr '&' expr	{ do_binary_op(ZEND_BW_AND, &$$, &$1, &$3 CLS_CC); }
	|	expr '^' expr	{ do_binary_op(ZEND_BW_XOR, &$$, &$1, &$3 CLS_CC); }
	|	expr '.' expr 	{ do_binary_op(ZEND_CONCAT,&$$,&$1,&$3 CLS_CC); }
	|	expr '+' expr 	{ do_binary_op(ZEND_ADD,&$$,&$1,&$3 CLS_CC); }
	|	expr '-' expr 	{ do_binary_op(ZEND_SUB,&$$,&$1,&$3 CLS_CC); }
	|	expr '*' expr	{ do_binary_op(ZEND_MUL,&$$,&$1,&$3 CLS_CC); }
	|	expr '/' expr	{ do_binary_op(ZEND_DIV,&$$,&$1,&$3 CLS_CC); }
	|	expr '%' expr 	{ do_binary_op(ZEND_MOD,&$$,&$1,&$3 CLS_CC); }
	| 	expr SHIFT_LEFT expr { do_binary_op(ZEND_SL, &$$, &$1, &$3 CLS_CC); }
	|	expr SHIFT_RIGHT expr { do_binary_op(ZEND_SR, &$$, &$1, &$3 CLS_CC); }
	|	'+' expr { $1.u.constant.value.lval=0; $1.u.constant.type=IS_LONG; $1.op_type = IS_CONST; $1.u.constant.refcount=1; do_binary_op(ZEND_ADD, &$$, &$1, &$2 CLS_CC); }
	|	'-' expr { $1.u.constant.value.lval=0; $1.u.constant.type=IS_LONG; $1.op_type = IS_CONST; $1.u.constant.refcount=1; do_binary_op(ZEND_SUB, &$$, &$1, &$2 CLS_CC); }
	|	'!' expr { do_unary_op(ZEND_BOOL_NOT, &$$, &$2 CLS_CC); }
	|	'~' expr { do_unary_op(ZEND_BW_NOT, &$$, &$2 CLS_CC); }
	|	expr IS_EQUAL expr				{ do_binary_op(ZEND_IS_EQUAL, &$$, &$1, &$3 CLS_CC); }
	|	expr IS_NOT_EQUAL expr 			{ do_binary_op(ZEND_IS_NOT_EQUAL, &$$, &$1, &$3 CLS_CC); }
	|	expr '<' expr 					{ do_binary_op(ZEND_IS_SMALLER, &$$, &$1, &$3 CLS_CC); }
	|	expr IS_SMALLER_OR_EQUAL expr 	{ do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$1, &$3 CLS_CC); }
	|	expr '>' expr 					{ do_binary_op(ZEND_IS_SMALLER, &$$, &$3, &$1 CLS_CC); }
	|	expr IS_GREATER_OR_EQUAL expr 	{ do_binary_op(ZEND_IS_SMALLER_OR_EQUAL, &$$, &$3, &$1 CLS_CC); }
	|	'(' expr ')' 	{ $$ = $2; }
	|	expr '?' { do_begin_qm_op(&$1, &$2 CLS_CC); }
		expr ':' { do_qm_true(&$4, &$2, &$5 CLS_CC); }
		expr	 { do_qm_false(&$$, &$7, &$2, &$5 CLS_CC); }
	|	STRING	'(' { do_extended_fcall_begin(CLS_C); do_begin_function_call(&$1 CLS_CC); }
				function_call_parameter_list
				')' { do_end_function_call(&$1, &$$, &$4, 0 CLS_CC); do_extended_fcall_end(CLS_C); }
	|	r_cvar '(' { do_extended_fcall_begin(CLS_C); do_begin_dynamic_function_call(&$1 CLS_CC); } 
				function_call_parameter_list 
				')' { do_end_function_call(&$1, &$$, &$4, 0 CLS_CC); do_extended_fcall_end(CLS_C);}
	|	STRING T_PAAMAYIM_NEKUDOTAYIM STRING '(' { do_extended_fcall_begin(CLS_C); do_begin_class_member_function_call(&$1, &$3 CLS_CC); } 
											function_call_parameter_list 
											')' { do_end_function_call(&$3, &$$, &$6, 1 CLS_CC); do_extended_fcall_end(CLS_C);}
	|	internal_functions_in_yacc { $$ = $1; }
	|	INT_CAST expr 		{ do_cast(&$$, &$2, IS_LONG CLS_CC); }
	|	DOUBLE_CAST expr 	{ do_cast(&$$, &$2, IS_DOUBLE CLS_CC); }
	|	STRING_CAST expr	{ do_cast(&$$, &$2, IS_STRING CLS_CC); } 
	|	ARRAY_CAST expr 	{ do_cast(&$$, &$2, IS_ARRAY CLS_CC); }
	|	OBJECT_CAST expr 	{ do_cast(&$$, &$2, IS_OBJECT CLS_CC); }
	|	T_EXIT exit_expr	{ do_exit(&$$, &$2 CLS_CC); }
	|	'@' { do_begin_silence(&$1 CLS_CC); } expr { do_end_silence(&$1 CLS_CC); $$ = $3; }
	|	scalar				{ $$ = $1; }
	|	ZEND_ARRAY '(' array_pair_list ')' { $$ = $3; }
	|	'`' encaps_list '`'		{ do_shell_exec(&$$, &$2 CLS_CC); }
	|	ZEND_PRINT expr  { do_print(&$$, &$2 CLS_CC); }
;


exit_expr:
		/* empty */		{ $$.op_type = IS_UNUSED; }	
	|	'(' ')'			{ $$.op_type = IS_UNUSED; }	
	|	'(' expr ')'	{ $$ = $2; }
;


ctor_arguments:
		/* empty */									{ $$.u.constant.value.lval=0; }
	|	'(' function_call_parameter_list ')'		{ $$ = $2; }
;


class_name:
		STRING	{ $$ = $1; }
	|	r_cvar	{ $$ = $1; }
;



common_scalar:
		LNUMBER 					{ $$=$1; }
	|	DNUMBER 					{ $$=$1; }
	|	CONSTANT_ENCAPSED_STRING	{ $$ = $1; }
	|	ZEND_LINE 					{ $$ = $1; }
	|	ZEND_FILE 					{ $$ = $1; }
;


static_scalar: /* compile-time evaluated scalars */
		common_scalar		{ $$ = $1; }
	|	STRING 				{ do_fetch_constant(&$$, &$1, ZEND_CT CLS_CC); }
	|	'+' static_scalar	{ $$ = $1; }
	|	'-' static_scalar	{ zval minus_one;  minus_one.type = IS_LONG; minus_one.value.lval = -1;  mul_function(&$2.u.constant, &$2.u.constant, &minus_one);  $$ = $2; }
	|	ZEND_ARRAY '(' static_array_pair_list ')' { $$ = $3; }
;


scalar:
		STRING 					{ do_fetch_constant(&$$, &$1, ZEND_RT CLS_CC); }
	|	common_scalar			{ $$ = $1; }
	|	'"' encaps_list '"' 	{ $$ = $2; }
	|	'\'' encaps_list '\''	{ $$ = $2; }
	|	ZEND_HEREDOC encaps_list ZEND_HEREDOC { $$ = $2; do_end_heredoc(CLS_C); }
;


static_array_pair_list:
		/* empty */ 						{ $$.op_type = IS_CONST; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; array_init(&$$.u.constant); }
	|	non_empty_static_array_pair_list	{ $$ = $1; }
;


non_empty_static_array_pair_list:
		non_empty_static_array_pair_list ',' static_scalar ZEND_DOUBLE_ARROW static_scalar	{ do_add_static_array_element(&$$, &$3, &$5); }
	|	non_empty_static_array_pair_list ',' static_scalar		{ do_add_static_array_element(&$$, &$3, NULL); }
	|	static_scalar ZEND_DOUBLE_ARROW static_scalar	{ $$.op_type = IS_CONST; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; array_init(&$$.u.constant); do_add_static_array_element(&$$, &$1, &$3); }
	|	static_scalar 									{ $$.op_type = IS_CONST; $$.u.constant.refcount=1; $$.u.constant.is_ref=0; array_init(&$$.u.constant); do_add_static_array_element(&$$, &$1, NULL); }
;

expr:
		r_cvar					{ $$ = $1; }
	|	expr_without_variable	{ $$ = $1; }
;



r_cvar:
	cvar { do_end_variable_parse(BP_VAR_R CLS_CC); $$ = $1; }
;


w_cvar:
	cvar { do_end_variable_parse(BP_VAR_W CLS_CC); $$ = $1; }
;


rw_cvar:
	cvar { do_end_variable_parse(BP_VAR_RW CLS_CC); $$ = $1; }
;


cvar:
		cvar_without_objects { $$ = $1; }
	|	cvar_without_objects ZEND_OBJECT_OPERATOR { do_push_object(&$1 CLS_CC); } ref_list { $$ = $4; }
;


cvar_without_objects:
		reference_variable { $$ = $1; }
	|	simple_indirect_reference reference_variable { do_indirect_references(&$$, &$1, &$2 CLS_CC); }
;


reference_variable:
		dim_list ']' { $$ = $1; }
	|	compound_variable		{ do_fetch_globals(&$1 CLS_CC); do_begin_variable_parse(CLS_C); fetch_simple_variable(&$$, &$1, 1 CLS_CC); }
;
	

compound_variable:
		VARIABLE			{ $$ = $1; }
	|	'$' '{' expr '}'	{ $$ = $3; }
;


dim_list:
		dim_list ']' '[' dim_offset	{ fetch_array_dim(&$$, &$1, &$4 CLS_CC); }
	|	compound_variable  { do_fetch_globals(&$1 CLS_CC); do_begin_variable_parse(CLS_C); } '[' dim_offset		{ fetch_array_begin(&$$, &$1, &$4 CLS_CC); }
;


dim_offset:
		/* empty */		{ $$.op_type = IS_UNUSED; }
	|	expr			{ $$ = $1; }
;


ref_list:
		object_property  { $$ = $1; }
	|	ref_list ZEND_OBJECT_OPERATOR { do_push_object(&$1 CLS_CC); } object_property { $$ = $4; }
;

object_property:
		scalar_object_property		{ znode tmp_znode;  do_pop_object(&tmp_znode CLS_CC);  do_fetch_property(&$$, &tmp_znode, &$1 CLS_CC); }
	|	object_dim_list ']' { $$ = $1; }
;

scalar_object_property:
		STRING			{ $$ = $1; }
	|	'{' expr '}'	{ $$ = $2; }
	|	cvar_without_objects { do_end_variable_parse(BP_VAR_R CLS_CC); $$ = $1; }
;


object_dim_list:
		object_dim_list ']' '[' expr { fetch_array_dim(&$$, &$1, &$4 CLS_CC); }
	|	STRING { znode tmp_znode, res;  do_pop_object(&tmp_znode CLS_CC);  do_fetch_property(&res, &tmp_znode, &$1 CLS_CC);  $1 = res; } '[' expr { fetch_array_dim(&$$, &$1, &$4 CLS_CC); }
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
		w_cvar								{ do_add_list_element(&$1 CLS_CC); }
	|	ZEND_LIST '(' { do_new_list_begin(CLS_C); } assignment_list ')'	{ do_new_list_end(CLS_C); }
	|	/* empty */							{ do_add_list_element(NULL CLS_CC); }
;


array_pair_list:
		/* empty */ 				{ do_init_array(&$$, NULL, NULL CLS_CC); }
	|	non_empty_array_pair_list	{ $$ = $1; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' expr ZEND_DOUBLE_ARROW expr	{ do_add_array_element(&$$, &$5, &$3 CLS_CC); }
	|	non_empty_array_pair_list ',' expr		{ do_add_array_element(&$$, &$3, NULL CLS_CC); }
	|	expr ZEND_DOUBLE_ARROW expr	{ do_init_array(&$$, &$3, &$1 CLS_CC); }
	|	expr 						{ do_init_array(&$$, &$1, NULL CLS_CC); }
;


encaps_list:
		encaps_list encaps_var { do_end_variable_parse(BP_VAR_R CLS_CC);  do_add_variable(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list STRING						{ do_add_string(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list NUM_STRING					{ do_add_string(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list ENCAPSED_AND_WHITESPACE		{ do_add_string(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list CHARACTER 					{ do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list BAD_CHARACTER				{ do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list '['		{ $2.u.constant.value.chval = '['; do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list ']'		{ $2.u.constant.value.chval = ']'; do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list '{'		{ $2.u.constant.value.chval = '{'; do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list '}'		{ $2.u.constant.value.chval = '}'; do_add_char(&$$, &$1, &$2 CLS_CC); }
	|	encaps_list ZEND_OBJECT_OPERATOR  { znode tmp;  $2.u.constant.value.chval = '-';  do_add_char(&tmp, &$1, &$2 CLS_CC);  $2.u.constant.value.chval = '>'; do_add_char(&$$, &tmp, &$2 CLS_CC); }
	|	/* empty */			{ do_init_string(&$$ CLS_CC); }

;



encaps_var:
		VARIABLE { do_fetch_globals(&$1 CLS_CC); do_begin_variable_parse(CLS_C); fetch_simple_variable(&$$, &$1, 1 CLS_CC); }
	|	VARIABLE '[' { do_begin_variable_parse(CLS_C); } encaps_var_offset ']'	{ do_fetch_globals(&$1 CLS_CC);  fetch_array_begin(&$$, &$1, &$4 CLS_CC); }
	|	VARIABLE ZEND_OBJECT_OPERATOR STRING { do_begin_variable_parse(CLS_C); fetch_simple_variable(&$2, &$1, 1 CLS_CC); do_fetch_property(&$$, &$2, &$3 CLS_CC); }
	|	DOLLAR_OPEN_CURLY_BRACES expr '}' { do_begin_variable_parse(CLS_C);  fetch_simple_variable(&$$, &$2, 1 CLS_CC); }
	|	DOLLAR_OPEN_CURLY_BRACES STRING '[' expr ']' '}' { do_begin_variable_parse(CLS_C);  fetch_array_begin(&$$, &$2, &$4 CLS_CC); }
	|	T_CURLY_OPEN cvar '}' { $$ = $2; }
;


encaps_var_offset:
		STRING	{ $$ = $1; }
	|	NUM_STRING	{ $$ = $1; }
	|	VARIABLE { fetch_simple_variable(&$$, &$1, 1 CLS_CC); }
;


internal_functions_in_yacc:
		T_ISSET '(' cvar ')'	{ do_isset_or_isempty(ZEND_ISSET, &$$, &$3 CLS_CC); }
	|	T_EMPTY '(' cvar ')'	{ do_isset_or_isempty(ZEND_ISEMPTY, &$$, &$3 CLS_CC); }
	|	INCLUDE expr 		{ do_include_or_eval(ZEND_INCLUDE, &$$, &$2 CLS_CC); }
	|	EVAL '(' expr ')' 	{ do_include_or_eval(ZEND_EVAL, &$$, &$3 CLS_CC); }
;


%%

