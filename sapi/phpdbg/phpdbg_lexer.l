%{
 
/*
 * phpdbg_lexer.l
 */

#include "phpdbg.h"
#include "phpdbg_cmd.h"
#define YYSTYPE phpdbg_param_t

#include "phpdbg_parser.h"

%}

%s RAW 
%s NORMAL

%option outfile="sapi/phpdbg/phpdbg_lexer.c" header-file="sapi/phpdbg/phpdbg_lexer.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

T_TRUE				"true"
T_YES				"yes"
T_ON				"on"
T_ENABLED			"enabled"
T_FALSE				"false"
T_NO				"no"
T_OFF				"off"
T_DISABLED			"disabled"
T_EVAL				"ev"
T_SHELL				"sh"
T_IF				"if"
T_RUN				"run"
T_RUN_SHORT			"r"

WS          		[ \r\n\t]+
DIGITS				[0-9\.]+
ID					[^ \r\n\t:#]+
ADDR				0x[a-fA-F0-9]+
OPCODE				(ZEND_|zend_)([A-Za-z])+
INPUT				[^\n]+
%%

<INITIAL>{
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
	{T_RUN}|{T_RUN_SHORT}	{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return T_RUN;
	}

	.+	{
		BEGIN(NORMAL);
		REJECT;
	}
}

<NORMAL>{
	{T_IF}			{
		BEGIN(RAW);
		phpdbg_init_param(yylval, EMPTY_PARAM);
		return T_IF;
	}
}

<INITIAL,NORMAL>{
	{ID}[:]{1}[//]{2}	{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = zend_strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_PROTO;
	}
	[#]{1}			{ return T_POUND;  }
	[:]{2}			{ return T_DCOLON; }
	[:]{1}			{ return T_COLON;  }

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
		yylval->addr = strtoul(yytext, 0, 16);
		return T_ADDR;
	}
	{OPCODE}	{
		phpdbg_init_param(yylval, OP_PARAM);
		yylval->str = zend_strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_OPCODE;
	}
	{ID}			{
		phpdbg_init_param(yylval, STR_PARAM);
		yylval->str = zend_strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_ID; 
	}
}

<RAW>{INPUT}		{
	phpdbg_init_param(yylval, STR_PARAM);
	yylval->str = zend_strndup(yytext, yyleng);
	yylval->len = yyleng;
	BEGIN(INITIAL);
	return T_INPUT;
}

{WS}        		{ /* ignore whitespace */ }
%%
