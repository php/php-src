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

%option outfile="phpdbg_lexer.c" header-file="phpdbg_lexer.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

C_TRUE		"true"
C_YES		"yes"
C_ON		"on"
C_ENABLED	"enabled"
C_FALSE		"false"
C_NO		"no"
C_OFF		"off"
C_DISABLED	"disabled"
C_EVAL		"eval"
C_SHELL		"shell"

DIGITS		[0-9]+
ID			[a-zA-Z][a-zA-Z0-9_]+
METHOD		{ID}::{ID}
FILE		[^ :0-9]{1,}:[0-9]+
OPLINE		0x[a-fA-F0-9]+
LITERAL		\"(\\.|[^\\"])*\"
WS          [ \r\n\t]+
INPUT		[^\n]+
%%
{WS}            							{ }
<INITIAL>
{
	{C_YES}|{C_ON}|{C_ENABLED}|{C_TRUE} 	{
		yylval->type = NUMERIC_PARAM;
		yylval->num = 1;		
		return C_TRUTHY;
	}
	{C_NO}|{C_OFF}|{C_DISABLED}|{C_FALSE} {
		yylval->type = NUMERIC_PARAM;
		yylval->num = 0;
		return C_FALSY;
	}
	{C_EVAL}		{ 
		BEGIN(RAW);
		return C_EVAL; 
	}
	{C_SHELL}		{ 
		BEGIN(RAW);
		return C_SHELL; 
	}
	{DIGITS}         { 
		yylval->type = NUMERIC_PARAM;
		yylval->num = atoi(yytext);
		return T_DIGITS;
	}
	{METHOD}		{
		yylval->type = METHOD_PARAM;
		yylval->method.class = "class";
		yylval->method.name = "func";
		return T_METHOD; 
	}
	{FILE}			{
		yylval->type = FILE_PARAM;
		yylval->file.name = strndup(yytext, yyleng);
		yylval->file.line = 0;
		return T_FILE;
	}
	{OPLINE}		{
		yylval->type = ADDR_PARAM;
		yylval->addr = strtoul(yytext, NULL, 10);
		return T_OPLINE; 
	}
	{LITERAL}		{ 
		yylval->type = STR_PARAM;
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_LITERAL; 
	}
	{ID}		{
		yylval->type = STR_PARAM;
		yylval->str = strndup(yytext, yyleng);
		yylval->len = yyleng;
		return T_ID; 
	}
}
<RAW>{INPUT}		{
	yylval->type = STR_PARAM;
	yylval->str = strndup(yytext, yyleng);
	yylval->len = yyleng;
	BEGIN(INITIAL);
	return T_INPUT;
}
.			{
	/** command detection here **/
}
%%
