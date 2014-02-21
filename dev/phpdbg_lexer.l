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

T_TRUE				?i:"true"
T_YES				?i:"yes"
T_ON				?i:"on"
T_ENABLED			?i:"enabled"
T_FALSE				?i:"false"
T_NO				?i:"no"
T_OFF				?i:"off"
T_DISABLED			?i:"disabled"
T_EVAL				?i:"eval"
T_SHELL				?i:"shell"
T_IF				?i:"if"

WS          		[ \r\n\t]+
DIGITS				[0-9\.]+
ID					[^ \r\n\t:#~]+
ADDR				0x[a-fA-F0-9]+
OPCODE				ZEND_([A-Z])+
LITERAL				\"(\\.|[^\\"])*\"
INPUT				[^\n]+
%%
<INITIAL>
{
	[#]{1}		{ return T_POUND;  }
	[:]{2}		{ return T_DCOLON; }
	[:]{1}		{ return T_COLON;  }
	[~]{1}		{ return T_SQUIGGLE; }
	{T_EVAL}		{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return T_EVAL;
	}
	{T_SHELL}		{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return T_SHELL;
	}
	{T_IF}			{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return T_IF;
	}
	{T_YES}|{T_ON}|{T_ENABLED}|{T_TRUE} 	{
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = 1;		
		return T_TRUTHY;
	}
	{T_NO}|{T_OFF}|{T_DISABLED}|{T_FALSE} {
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = 0;
		return T_FALSY;
	}
	{DIGITS}         {
		phpdbg_init_param(yylval, NUMERIC_PARAM);
		yylval->num = atoi(yytext);
		return T_DIGITS;
	}
	{ADDR}		{
		phpdbg_init_param(yylval, ADDR_PARAM);
		yylval->addr = strtoul(yytext, NULL, 10);
		return T_ADDR; 
	}
	{OPCODE}	{
		phpdbg_init_param(yylval, OP_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_OPCODE;
	}
	{LITERAL}		{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_LITERAL; 
	}
	{ID}			{
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
%%
