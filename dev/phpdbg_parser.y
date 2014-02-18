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
#include "phpdbg_prompt.h"

#define YYSTYPE phpdbg_param_t

#include "phpdbg_parser.h"
#include "phpdbg_lexer.h"

ZEND_EXTERN_MODULE_GLOBALS(phpdbg);

int yyerror(phpdbg_param_t *stack, yyscan_t scanner, const char *msg) {
    phpdbg_error("Parse Error: %s", msg);
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
			
			case NUMERIC_METHOD_PARAM:
				fprintf(stderr, "%s NUMERIC_METHOD_PARAM(%s::%s)\n", msg, param->method.class, param->method.name);
			break;
			
			case NUMERIC_FUNCTION_PARAM:
				fprintf(stderr, "%s NUMERIC_FUNCTION_PARAM(%s::%s)\n", msg, param->str, param->num);
			break;
			
			case NUMERIC_PARAM:
				fprintf(stderr, "%s NUMERIC_PARAM(%ld)\n", msg, param->num);
			break;
			
			case COND_PARAM:
				fprintf(stderr, "%s COND_PARAM(%s=%d)\n", msg, param->str, param->len);
			break;
		}
	}
}

void phpdbg_stack_free(phpdbg_param_t *stack) {
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
	phpdbg_debug_param(next, "push ->");
	stack->len++;
}

phpdbg_command_t* phpdbg_stack_resolve(const phpdbg_command_t *commands, phpdbg_param_t **top, char **why) {
	const phpdbg_command_t *command = commands;
	phpdbg_param_t *name = *top;
	phpdbg_command_t *matched[3] = {NULL, NULL, NULL};
	ulong matches = 0L;
	
	while (command && command->name && command->handler) {
		if (command->name_len >= name->len) {
			if (memcmp(command->name, name->str, name->len) == SUCCESS) {
				if (matches < 3) {
					matched[matches] = command;
					matches++;
				} else break;
			}
		}
		command++;
	}
	
	switch (matches) {
		case 0: { 
			asprintf(
				why,
				"The command %s could not be found", 
				name->str);
		} break;
		
		case 1: {
			(*top) = (*top)->next;
			if (matched[0]->subs && (*top) && ((*top)->type == STR_PARAM)) {
				command = phpdbg_stack_resolve(matched[0]->subs, top, why);
				if (command) {
					phpdbg_notice(
						"Command matching with sub command %s %s", 
						matched[0]->name, command->name);
					return command;
				}
			}
			
			phpdbg_notice(
				"Command matching with %s", 
				matched[0]->name);
			return matched[0];
		} break;
		
		default: {
			asprintf(
				why,
				"The command %s is ambigious, matching %d commands", 
				name->str, matches);
		}
	}
	
	return NULL;
}

int phpdbg_stack_execute(phpdbg_param_t *stack, char **why) {
	phpdbg_param_t *command = NULL,
				   *params = NULL;
	phpdbg_command_t *handler = NULL;
	
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
	
	command = (phpdbg_param_t*) stack->next;
	
	switch (command->type) {
		case EVAL_PARAM:
			PHPDBG_COMMAND_HANDLER(eval)(command, NULL TSRMLS_CC);
		break;
		
		case SHELL_PARAM:
			PHPDBG_COMMAND_HANDLER(shell)(command, NULL TSRMLS_CC);
		break;
		
		case STR_PARAM: {
			/* do resolve command(s) */
			handler = phpdbg_stack_resolve(
				phpdbg_prompt_commands, &command, why);
			
			if (handler) {
				/* get top of stack */
				params = command;
				
				/* prepare params */
				while (params) {
					phpdbg_debug_param(params, "-> ...");
					params = params->next;
				}
				
				return SUCCESS;
			} else {
				return FAILURE;
			}
		} break;
		
		default:
			asprintf(
				why, "the first parameter makes no sense !!");
			return FAILURE;
	}
	
	return SUCCESS;
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
