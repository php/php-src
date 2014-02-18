%{
 
/*
 * phpdbg_lexer.l
 */

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#define YYSTYPE phpdbg_param_t

#include "phpdbg_parser.h"
#include <stdio.h>
#include <string.h>
%}

%s RAW

%option outfile="sapi/phpdbg/phpdbg_lexer.c" header-file="sapi/phpdbg/phpdbg_lexer.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

C_TRUE				?i:"true"
C_YES				?i:"yes"
C_ON				?i:"on"
C_ENABLED			?i:"enabled"
C_FALSE				?i:"false"
C_NO				?i:"no"
C_OFF				?i:"off"
C_DISABLED			?i:"disabled"
C_EVAL				?i:"eval"
C_SHELL				?i:"shell"
C_IF				?i:"if"

DIGITS				[0-9]+
ID					[a-zA-Z_\x7f-\xff\-][a-zA-Z0-9_\x7f-\xff\-\./]*
NSID				[\\\\]?{ID}
METHOD				{NSID}+::{ID}
NUMERIC_METHOD		{METHOD}[#]{DIGITS}
NUMERIC_FUNCTION	{NSID}[#]{DIGITS}
FILE				[^ :]+:[0-9]+
OPLINE				0x[a-fA-F0-9]+
LITERAL				\"(\\.|[^\\"])*\"
WS          		[ \r\n\t]+
INPUT				[^\n]+
%%
<INITIAL>
{
	{C_EVAL}		{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return C_EVAL;
	}
	{C_SHELL}		{ 
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return C_SHELL;
	}
	{C_IF}			{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return C_IF;
	}
	{C_YES}|{C_ON}|{C_ENABLED}|{C_TRUE} 	{
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = 1;		
		return C_TRUTHY;
	}
	{C_NO}|{C_OFF}|{C_DISABLED}|{C_FALSE} {
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = 0;
		return C_FALSY;
	}
	{DIGITS}         {
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = atoi(yytext);
		return T_DIGITS;
	}
	{NUMERIC_METHOD}	{
		phpdbg_init_param(yylval, NUMERIC_METHOD_PARAM);
		yylval->method.class = "class";
		yylval->method.name = "func";
		yylval->num = 0;
		return T_METHOD; 
	}
	{NUMERIC_FUNCTION}	{
		phpdbg_init_param(yylval, NUMERIC_FUNCTION_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		yylval->num = 0;
		return T_ID; 
	}
	{METHOD}		{
		phpdbg_init_param(yylval, METHOD_PARAM);
		yylval->method.class = "class";
		yylval->method.name = "func";
		return T_METHOD; 
	}
	{FILE}			{
		phpdbg_init_param(yylval, FILE_PARAM);
		yylval->file.name = strndup(yytext, yyleng);
		yylval->file.line = 0;
		return T_FILE;
	}
	{OPLINE}		{
		phpdbg_init_param(yylval, ADDR_PARAM);
		yylval->addr = strtoul(yytext, NULL, 10);
		return T_OPLINE; 
	}
	{LITERAL}		{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_LITERAL; 
	}
	{NSID}			{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_ID; 
	}
}
<RAW>{INPUT}		{
	phpdbg_init_param(yylval, STR_PARAM);
	yylval->str = strndup(yytext, yyleng);
	yylval->len = yyleng;
	BEGIN(INITIAL);
	return T_INPUT;
}
{WS}        		{ /* ignore whitespace */ }
.			{
	phpdbg_init_param(yylval, EMPTY_PARAM);
	return T_UNEXPECTED; 
}
%%
