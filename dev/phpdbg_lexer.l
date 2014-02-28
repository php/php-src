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

static inline void phpdbg_append_string(phpdbg_param_t *param, const char *string, size_t length TSRMLS_DC) {
	if (!param->str) {
		param->str = malloc(length+1);
	} else param->str = realloc(param->str, param->len + (length+1));
	
	memcpy(&param->str[param->len], string, length);
	param->len += length;
	param->str[param->len] = 0;
}
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
T_EVAL				?i:"ev"
T_SHELL				?i:"sh"
T_IF				?i:"if"

WS          		[ \r\n\t]+
DIGITS				[0-9\.]+
ID					[^ \r\n\t:#]+
ADDR				0x[a-fA-F0-9]+
OPCODE				?i:ZEND_([A-Za-z])+
INPUT				[^\n]+
%%
<INITIAL>{
	{ID}[:]{1}[//]{2}	{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_PROTO;
	}
	[#]{1}			{ return T_POUND;  }
	[:]{2}			{ return T_DCOLON; }
	[:]{1}			{ return T_COLON;  }
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
