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

#define YYSTYPE w32api_parser_type_definition_union 
#define YYPARSE_PARAM th

int w32api_type_definition_lex(w32api_parser_type_definition_union *typedef_lval);

%}

%pure_parser

%token <s> tTYPEID
%token tBYREF

%type <type> value_list value

%%

start:
	type_definition
;

type_definition:
	tTYPEID '{' value_list '}' {((w32api_type_handle_ptr *)th)->hnd = w32api_parser_register_type($1, $3);}
;

value_list:
	value ';' value_list {$$ = w32api_parser_type_join_values($1, $3);}
	| value ';' {$$ = $1;} 
;

value:
	tTYPEID tTYPEID {$$ = w32api_parser_type_make_byval_value($1, $2);}
	| tTYPEID tBYREF tTYPEID {$$ = w32api_parser_type_make_byref_value($1, $3);} 
;

