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

#undef yyerror
static int yyerror(const char *msg);

ZEND_EXTERN_MODULE_GLOBALS(phpdbg)

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif

%}

%pure-parser
%error-verbose

%code requires {
#include "phpdbg.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}

%output  "sapi/phpdbg/phpdbg_parser.c"
%defines "sapi/phpdbg/phpdbg_parser.h"

%token T_EVAL       "eval"
%token T_RUN        "run"
%token T_SHELL      "shell"
%token T_IF         "if (condition)"
%token T_TRUTHY     "truthy (true, on, yes or enabled)"
%token T_FALSY      "falsy (false, off, no or disabled)"
%token T_STRING     "string (some input, perhaps)"
%token T_COLON      ": (colon)"
%token T_DCOLON     ":: (double colon)"
%token T_POUND      "# (pound sign followed by digits)"
%token T_SEPARATOR  "# (pound sign)"
%token T_PROTO      "protocol (file://)"
%token T_DIGITS     "digits (numbers)"
%token T_LITERAL    "literal (string)"
%token T_ADDR       "address"
%token T_OPCODE     "opcode"
%token T_ID         "identifier (command or function name)"
%token T_INPUT      "input (input string or data)"
%token T_UNEXPECTED "input"
%token T_REQ_ID     "request id (-r %d)"

%% /* Rules */

input
	: command { $$ = $1; }
	| input T_SEPARATOR command { phpdbg_stack_separate($1.top); $$ = $3; }
	| /* nothing */
	;

command
	: parameters { $$.top = PHPDBG_G(parser_stack)->top; }
	| full_expression { phpdbg_stack_push(PHPDBG_G(parser_stack), &$1); $$.top = PHPDBG_G(parser_stack)->top; }
	;

parameters
	: parameter { phpdbg_stack_push(PHPDBG_G(parser_stack), &$1); $$.top = PHPDBG_G(parser_stack)->top; }
	| parameters parameter { phpdbg_stack_push(PHPDBG_G(parser_stack), &$2); $$.top = PHPDBG_G(parser_stack)->top; }
	| parameters req_id { $$ = $1; }
	;

parameter
	: T_ID T_COLON T_DIGITS {
		$$.type = FILE_PARAM;
		$$.file.name = $2.str;
		$$.file.line = $3.num;
	}
	| T_ID T_COLON T_POUND T_DIGITS {
		$$.type = NUMERIC_FILE_PARAM;
		$$.file.name = $1.str;
		$$.file.line = $4.num;
	}
	| T_PROTO T_ID T_COLON T_DIGITS {
		$$.type = FILE_PARAM;
		$$.file.name = malloc($1.len + $2.len + 1);
		if ($$.file.name) {
			memcpy(&$$.file.name[0], $1.str, $1.len);
			memcpy(&$$.file.name[$1.len], $2.str, $2.len);
			$$.file.name[$1.len + $2.len] = '\0';
		}
		$$.file.line = $4.num;
	}
	| T_PROTO T_ID T_COLON T_POUND T_DIGITS {
		$$.type = NUMERIC_FILE_PARAM;
		$$.file.name = malloc($1.len + $2.len + 1);
		if ($$.file.name) {
			memcpy(&$$.file.name[0], $1.str, $1.len);
			memcpy(&$$.file.name[$1.len], $2.str, $2.len);
			$$.file.name[$1.len + $2.len] = '\0';
		}
		$$.file.line = $5.num;
	}
	| T_ID T_DCOLON T_ID {
		$$.type = METHOD_PARAM;
		$$.method.class = $1.str;
		$$.method.name = $3.str;
	}
	| T_ID T_DCOLON T_ID T_POUND T_DIGITS {
		$$.type = NUMERIC_METHOD_PARAM;
		$$.method.class = $1.str;
		$$.method.name = $3.str;
		$$.num = $5.num;
	}
	| T_ID T_POUND T_DIGITS {
		$$.type = NUMERIC_FUNCTION_PARAM;
		$$.str = $1.str;
		$$.len = $1.len;
		$$.num = $3.num;
	}
	| T_IF T_INPUT {
		$$.type = COND_PARAM;
		$$.str = $2.str;
		$$.len = $2.len;
	}
	| T_OPCODE { $$ = $1; }
	| T_ADDR { $$ = $1; }
	| T_LITERAL { $$ = $1; }
	| T_TRUTHY { $$ = $1; }
	| T_FALSY { $$ = $1; }
	| T_DIGITS { $$ = $1; }
	| T_ID { $$ = $1; }
	;

req_id
	: T_REQ_ID { PHPDBG_G(req_id) = $1.num; }
	| /* empty */
;

full_expression
	: T_EVAL req_id T_INPUT {
		$$.type = EVAL_PARAM;
		$$.str = $3.str;
		$$.len = $3.len;
	}
	| T_SHELL req_id T_INPUT {
		$$.type = SHELL_PARAM;
		$$.str = $3.str;
		$$.len = $3.len;
	}
	| T_RUN req_id {
		$$.type = RUN_PARAM;
		$$.len = 0;
	}
	| T_RUN req_id T_INPUT {
		$$.type = RUN_PARAM;
		$$.str = $3.str;
		$$.len = $3.len;
	}
	;

%%

static int yyerror(const char *msg) {
	phpdbg_error("command", "type=\"parseerror\" msg=\"%s\"", "Parse Error: %s", msg);

	{
		const phpdbg_param_t *top = PHPDBG_G(parser_stack);

		while (top) {
			phpdbg_param_debug(top, "--> ");
			top = top->next;
		}
	}
	return 0;
}

int phpdbg_do_parse(phpdbg_param_t *stack, char *input) {
	if (!*input) {
		return 0;
	}

	phpdbg_init_lexer(stack, input);

	return yyparse();
}
