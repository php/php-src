%error-verbose
%{
 
/*
 * phpdbg_parser.y
 * (from php-src root)
 * flex sapi/phpdbg/dev/phpdbg_lexer.l
 * bison sapi/phpdbg/dev/phpdbg_parser.y
 */
 
#include "phpdbg.h"
#include "phpdbg_cmd.h"
#include "phpdbg_utils.h"
#include "phpdbg_cmd.h"
#include "phpdbg_prompt.h"

#define YYSTYPE phpdbg_param_t

#include "phpdbg_parser.h"
#include "phpdbg_lexer.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

int yyerror(phpdbg_param_t *stack, yyscan_t scanner, const char *msg) {
    phpdbg_error("Parse Error: %s", msg);
    {
    	const phpdbg_param_t *top = stack;
    	zend_ulong position  = 0L;
    	
    	while (top) {
    		phpdbg_param_debug(
    			top, "--> ");
    		top = top->next;
    	}
    }
}
%}
 
%code requires {
#include "phpdbg.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}
%expect 1
%output  "sapi/phpdbg/phpdbg_parser.c"
%defines "sapi/phpdbg/phpdbg_parser.h"
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { phpdbg_param_t *stack }
%parse-param { yyscan_t scanner }

%token C_TRUTHY		"truthy (true, on, yes or enabled)"
%token C_FALSY		"falsy (false, off, no or disabled)"
%token C_STRING		"string (some input, perhaps)"
%token C_EVAL	 	"eval"
%token C_SHELL		"shell"
%token C_IF			"if (condition)"

%token T_DIGITS	 			"digits (numbers)"
%token T_LITERAL 			"literal (string)"
%token T_METHOD	 			"method"
%token T_NUMERIC_METHOD		"method opline address"
%token T_NUMERIC_FUNCTION	"function opline address"
%token T_OPLINE	 			"opline"
%token T_FILE	 			"file"
%token T_ID		 			"identifier (command or function name)"
%token T_INPUT	 			"input (input string or data)"
%token T_UNEXPECTED 		"input"
%%

input
    : handler
    ;

parameters
	: parameter								{ phpdbg_stack_push(stack, &$1); }
	| parameters parameter					{ phpdbg_stack_push(stack, &$2); }
	;

params
	: parameters
	| /* empty */
	;

normal
	:	T_ID								{ phpdbg_stack_push(stack, &$1); }
	|	normal T_ID							{ phpdbg_stack_push(stack, &$2); }
	;
	
special
	: C_EVAL T_INPUT                        { $$ = $2; $$.type = EVAL_PARAM;  }
	| C_SHELL T_INPUT						{ $$ = $2; $$.type = SHELL_PARAM; }
	;

command
	: normal
	| special								{ phpdbg_stack_push(stack, &$1); }
	;
	
parameter
	: T_DIGITS								{ $$ = $1; }
	| T_FILE								{ $$ = $1; }
	| T_METHOD								{ $$ = $1; }
	| T_NUMERIC_METHOD						{ $$ = $1; }
	| T_NUMERIC_FUNCTION					{ $$ = $1; }
	| T_OPLINE								{ $$ = $1; }
	| T_ID									{ $$ = $1; } 
	| T_LITERAL								{ $$ = $1; }
	| C_TRUTHY								{ $$ = $1; }
	| C_FALSY								{ $$ = $1; }
	| C_IF T_INPUT							{ $$ = $2; $$.type = COND_PARAM; }
	;

handler
	: command params
	;
%%
