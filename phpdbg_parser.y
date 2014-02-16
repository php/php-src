%error-verbose
%{
 
/*
 * phpdbg_parser.y
 *
 * flex phpdbg_lexer.l
 * bison phpdbg_parser.y
 * gcc -g -o parser phpdbg_lexer.c phpdbg_parser.c -I/usr/src/php-src/main -I/usr/src/php-src/Zend -I/usr/src/php-src/TSRM -I/usr/src/php-src
 */
 
#include "phpdbg.h"
#include "phpdbg_cmd.h"

#define YYSTYPE phpdbg_param_t

#include "phpdbg_parser.h"
#include "phpdbg_lexer.h"

int yyerror(phpdbg_param_t *stack, yyscan_t scanner, const char *msg) {
    fprintf(stderr, "Parse Error: %s\n", msg);
}

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

static void phpdbg_stack_free(phpdbg_param_t *stack) {
	if (stack && stack->next) {
		phpdbg_param_t *remove = stack->next;
		
		while (remove) {
			phpdbg_param_t *next = NULL;
			
			if (remove->next)
				next = remove->next;
			
			switch (remove->type) {
				case STR_PARAM: 
					if (remove->str) {
						free(remove->str);	
					}
				break;
				
				case FILE_PARAM:
					if (remove->file.name) {
						free(remove->file.name);
					}
				break;
			}
			
			free(remove);
			
			if (next) 
				remove = next; 
			else break;
		}
	}
}

static void phpdbg_stack_push(phpdbg_param_t *stack, phpdbg_param_t *param) {
	phpdbg_param_t *next = calloc(1, sizeof(phpdbg_param_t));
	
	if (!next)
		return;
	
	*(next) = *(param);

	if (stack->top == NULL) {
		stack->top = next;
		stack->next = next;
	} else {
		stack->top->next = next;
		next->top = stack->top;
		stack->top = next;
	}
	
	stack->len++;
}

static int phpdbg_stack_execute(phpdbg_param_t *stack, char **why) {
	phpdbg_param_t *command = NULL,
				   *params = NULL;
	
	if (stack->type != STACK_PARAM) {
		asprintf(
			why, "the passed argument was not a stack !!");
		return FAILURE;
	}
	
	if (!stack->len) {
		asprintf(
			why, "the stack contains nothing !!");
		return FAILURE;
	}
	
	command = params = (phpdbg_param_t*) stack->next;

	if (command->type != STR_PARAM) {
		asprintf(
			why, "the first parameter is expected to be a string !!");
		return FAILURE;
	}
	
	/* do resolve command(s) */
	
	/* do prepare params for function */
	while (params) {
		phpdbg_debug_param(params, "-> ...");
		params = params->next;
	}
	
	return SUCCESS;
}

int main(int argc, char **argv) {
 	do {
		yyscan_t scanner;
		YY_BUFFER_STATE state;
		char buffer[8096];
		size_t buflen = 0L;
		phpdbg_param_t stack;
		
		phpdbg_init_param(&stack, STACK_PARAM);
		
		if (fgets(&buffer[0], 8096, stdin) != NULL) {
			if (yylex_init(&scanner)) {
				fprintf(stderr, "could not initialize scanner\n");
				return 1;
			}

			state = yy_scan_string(buffer, scanner);
	 		
			if (yyparse(&stack, scanner) <= 0) {
				char *why = NULL;
				
				if (phpdbg_stack_execute(&stack, &why) != SUCCESS) {
					fprintf(stderr, 
						"Execution Error: %s\n", 
						why ? why : "for no particular reason");
				}
				
				if (why) {
					free(why);
				}
			}
			yy_delete_buffer(state, scanner);
			yylex_destroy(scanner);
		} else fprintf(stderr, "could not get input !!\n");
		
		phpdbg_stack_free(&stack);
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
%parse-param { phpdbg_param_t *stack }
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
    : handler
    ;

parameters
	: parameter								{ phpdbg_stack_push(stack, &$1); }
	| parameters parameter					{ phpdbg_stack_push(stack, &$2); }
	;

params
	: /* empty */
	| parameters
	;

normal
	:	T_ID								{ phpdbg_stack_push(stack, &$1); }
	|	normal T_ID							{ phpdbg_stack_push(stack, &$2); }
	;
	
special
	: C_EVAL T_INPUT                        { phpdbg_stack_push(stack, &$1); phpdbg_stack_push(stack, &$2); }
	| C_SHELL T_INPUT						{ phpdbg_stack_push(stack, &$1); phpdbg_stack_push(stack, &$2); }
	;

command
	: normal								
	| special								
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
	: command params
	;
%%
