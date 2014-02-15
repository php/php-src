%error-verbose
%{
 
/*
 * phpdbg_parser.y
 *
 * flex phpdb_lexer.l
 * bison phpdbg_parser.y
 * gcc -g -o parser phpdbg_lexer.c phpdbg_parser.c -I/usr/src/php-src/main -I/usr/src/php-src/Zend -I/usr/src/php-src/TSRM -I/usr/src/php-src
 */
 
#include "phpdbg.h"
#include "phpdbg_cmd.h"

#define YYSTYPE phpdbg_param_t

void phpdbg_debug_param(const phpdbg_param_t *param, const char *msg) {
	if (param && param->type) {
		switch (param->type) {
			case STR_PARAM:
				fprintf(stderr, "%s STR_PARAM(%s=%d)\n", msg, param->str, param->len);
			break;
			
			case ADDR_PARAM:
				fprintf(stderr, "%s ADDR_PARAM(%lu)\n", msg, param->addr);
			break;
			
			case FILE_PARAM:
				fprintf(stderr, "%s FILE_PARAM(%s:%d)\n", msg, param->file.name, param->file.line);
			break;
			
			case METHOD_PARAM:
				fprintf(stderr, "%s METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_PARAM:
				fprintf(stderr, "%s NUMERIC_PARAM(%ld)\n", msg, param->num);
			break;
		}
	}
}

#include "phpdbg_parser.h"
#include "phpdbg_lexer.h"

int yyerror(phpdbg_param_t **param, yyscan_t scanner, const char *msg) {
    fprintf(stderr, "Parse Error: %s\n", msg);
}

int main(int argc, char **argv) {
 	do {
 		phpdbg_param_t *expression;
		yyscan_t scanner;
		YY_BUFFER_STATE state;
		char buffer[8096];
		size_t buflen = 0L;
		
		if (fgets(&buffer[0], 8096, stdin) != NULL) {
			if (yylex_init(&scanner)) {
				// couldn't initialize
				fprintf(stderr, "could not initialize scanner\n");
				return 1;
			}

			state = yy_scan_string(buffer, scanner);
	 		
			if (yyparse(&expression, scanner) <= 0) {
				// error parsing
				yy_delete_buffer(state, scanner);
				yylex_destroy(scanner);
			} else fprintf(stderr, "could not parse input (%s) !!\n", buffer);
		} else fprintf(stderr, "could not get input !!\n");
 	} while (1);
 	
	return 0;
}
%}
 
%code requires {
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}
%expect 1 
%output  "phpdbg_parser.c"
%defines "phpdbg_parser.h"
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { phpdbg_param_t **expression }
%parse-param { yyscan_t scanner }

%token C_TRUTHY		"truthy (true, on, yes or enabled)"
%token C_FALSY		"falsy (false, off, no or disabled)"
%token C_STRING		"string (some input, perhaps)"
%token C_EVAL	 	"eval"
%token C_SHELL		"shell"

%token T_DIGITS	 "digits (numbers)"
%token T_LITERAL "literal (T_LITERAL)"
%token T_METHOD	 "method (T_METHOD)"
%token T_OPLINE	 "opline (T_OPLINE)"
%token T_FILE	 "file (T_FILE)"
%token T_ID		 "identifier (T_ID)"
%token T_INPUT	 "input (input string or data)"
%token T_UNEXPECTED "unexpected input (input string or data)"
%%

input
    : handler								{} 
    ;

parameters
	: parameters parameter					{ phpdbg_debug_param(&$2, "got another parameter"); }
	| parameter								{ phpdbg_debug_param(&$1, "got first parameter");   }
	;

params
	: /* empty */							{ /* do nothing */ }
	| parameters							{ $$ = $1; }
	;

command
	: T_ID 									{ fprintf(stderr, "got cmd: %s\n", $1.str);    				}
	| T_ID T_ID								{ fprintf(stderr, "got sub: %s -> %s\n", $1.str, $2.str); 	}
	/* exceptional cases below */
	| C_EVAL T_INPUT                        { fprintf(stderr, "got eval: %s\n", $2.str); 	            }
	| C_SHELL T_INPUT						{ fprintf(stderr, "got shell: %s\n", $2.str); 	            }
	;
	
parameter
	: T_DIGITS								{ $$ = $1; }
	| T_FILE								{ $$ = $1; }
	| T_METHOD								{ $$ = $1; }
	| T_OPLINE								{ $$ = $1; }
	| T_ID									{ $$ = $1; } 
	| T_LITERAL								{ $$ = $1; }
	| C_TRUTHY								{ $$ = $1; }
	| C_FALSY								{ $$ = $1; }
	;

handler
	: command params 						{}
	;
%%
