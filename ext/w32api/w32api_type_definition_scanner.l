%{
/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2006 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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
#include "w32api_type_definition_parser.h"

#ifdef YYSTYPE
#undef YYSTYPE
#endif

#define YYSTYPE w32api_parser_type_definition_union 

#define YY_DECL int w32api_type_definition_lex(w32api_parser_type_definition_union *typedef_lval)

%}

TYPEID [A-Za-z][A-Za-z0-9_]*

%option noyywrap

%%

"&"         {return tBYREF;}
{TYPEID}    {typedef_lval->s = estrdup(yytext); return tTYPEID;}
[ \r\t\n]   /* Ignore Whitespace */
.           {return *yytext;}

%%