%{
/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: James Moore <jmoore@php.net>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_w32api.h"


#define YYSTYPE w32api_parser_function_definition_union 
#define YYPARSE_PARAM fh

int w32api_function_definition_lex(w32api_parser_function_definition_union *funcdef_lval);

%}

%pure_parser

%token tFROM
%token tBYREF
%token tALIAS
%token <s> tIDENTIFIER
%token <s> tFILENAME

%type <arg> argument argument_list

%start funcdef

%%

funcdef:			tIDENTIFIER tIDENTIFIER tALIAS tIDENTIFIER'(' argument_list ')' tFROM tFILENAME {((w32api_func_handle_ptr *)fh)->hnd = w32api_parser_load_alias_function($1, $2, $4, $6, $9);}
					| tIDENTIFIER tIDENTIFIER '(' argument_list ')' tFROM tFILENAME {((w32api_func_handle_ptr *)fh)->hnd = w32api_parser_load_function($1, $2, $4, $7);}
;

argument_list:		argument ',' argument_list { $$ = w32api_parser_join_arguments($1, $3);}
					| argument {$$ = $1;}
					| {$$ = NULL}
;

argument:			tIDENTIFIER tIDENTIFIER { $$ = w32api_parser_make_argument_byval($1, $2); }
					| tIDENTIFIER tBYREF tIDENTIFIER { $$ = w32api_parser_make_argument_byref($1, $3); }
;
